//
// Created by Ricardo on 03/07/2024.
//

#ifndef DUK_TOOLS_GLOBALS_H
#define DUK_TOOLS_GLOBALS_H

#include <duk_macros/assert.h>
#include <duk_tools/types.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace duk::tools {

class Globals {
public:

    template<typename T, typename ...Args>
    T* make(Args&&... args);

    template<typename T>
    void reset();

    template<typename T>
    bool has() const;

    template<typename T>
    T* get() const;

private:

    class Global {
    public:
        virtual ~Global() = default;
    };

    template<typename T>
    class GlobalT final : public Global {
    public:

        template<typename ...Args>
        GlobalT(Args&&... args);

        T* get();

    private:
        T m_data;
    };

private:
    std::unordered_map<std::string, std::unique_ptr<Global>> m_globals;
};

template<typename T, typename ... Args>
T* Globals::make(Args&&... args) {
    DUK_ASSERT(!has<T>());
    m_globals.emplace(type_name_of<T>(), std::make_unique<GlobalT<T>>(std::forward<Args>(args)...));
    return get<T>();
}

template<typename T>
void Globals::reset() {
    m_globals.erase(type_name_of<T>());
}

template<typename T>
T* Globals::get() const {
    return static_cast<GlobalT<T>*>(m_globals.at(type_name_of<T>()).get())->get();
}

template<typename T>
bool Globals::has() const {
    return m_globals.contains(type_name_of<T>());
}

template<typename T>
template<typename ... Args>
Globals::GlobalT<T>::GlobalT(Args&&... args)
    : m_data(std::forward<Args>(args)...) {

}

template<typename T>
T* Globals::GlobalT<T>::get() {
    return &m_data;
}

}

#endif //DUK_TOOLS_GLOBALS_H
