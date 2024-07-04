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
    template<typename T, typename... Args>
    T* add(Args&&... args);

    template<typename T>
    T* add_external(T* data);

    template<typename T>
    void reset();

    template<typename T>
    bool has() const;

    template<typename T>
    T* get() const;

private:
    template<typename T>
    static uint64_t hash_of();

    class Global {
    public:
        virtual ~Global() = default;

        virtual void* get() = 0;
    };

    template<typename T>
    class GlobalT final : public Global {
    public:
        template<typename... Args>
        GlobalT(Args&&... args);

        void* get() override;

    private:
        T m_data;
    };

    template<typename T>
    class ExternalGlobalT final : public Global {
    public:
        ExternalGlobalT(T* data);

        void* get() override;

    private:
        T* m_data;
    };

private:
    std::unordered_map<uint64_t, std::unique_ptr<Global>> m_globals;
};

template<typename T, typename... Args>
T* Globals::add(Args&&... args) {
    DUK_ASSERT(!has<T>());
    m_globals.emplace(hash_of<T>(), std::make_unique<GlobalT<T>>(std::forward<Args>(args)...));
    return get<T>();
}

template<typename T>
T* Globals::add_external(T* data) {
    DUK_ASSERT(!has<T>());
    m_globals.emplace(hash_of<T>(), std::make_unique<ExternalGlobalT<T>>(data));
    return get<T>();
}

template<typename T>
void Globals::reset() {
    m_globals.erase(hash_of<T>());
}

template<typename T>
T* Globals::get() const {
    return static_cast<T*>(m_globals.at(hash_of<T>())->get());
}

template<typename T>
uint64_t Globals::hash_of() {
    static uint64_t hash = std::hash<std::string>()(type_name_of<T>());
    return hash;
}

template<typename T>
bool Globals::has() const {
    return m_globals.contains(hash_of<T>());
}

template<typename T>
template<typename... Args>
Globals::GlobalT<T>::GlobalT(Args&&... args)
    : m_data(std::forward<Args>(args)...) {
}

template<typename T>
void* Globals::GlobalT<T>::get() {
    return &m_data;
}

template<typename T>
Globals::ExternalGlobalT<T>::ExternalGlobalT(T* data)
    : m_data(data) {
}

template<typename T>
void* Globals::ExternalGlobalT<T>::get() {
    return m_data;
}

}// namespace duk::tools

#endif//DUK_TOOLS_GLOBALS_H
