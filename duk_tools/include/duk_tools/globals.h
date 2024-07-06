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
    ~Globals();

    void clear();

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

    static std::unordered_map<uint64_t, uint32_t>& hash_to_index();

    template<typename T>
    uint32_t index_of() const;

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
    std::vector<std::unique_ptr<Global>> m_globals;
};

template<typename T, typename... Args>
T* Globals::add(Args&&... args) {
    DUK_ASSERT(!has<T>());
    const auto index = index_of<T>();
    if (index >= m_globals.size()) {
        m_globals.resize(index + 1);
    }
    auto& global = m_globals[index] = std::make_unique<GlobalT<T>>(std::forward<Args>(args)...);
    return static_cast<T*>(global->get());
}

template<typename T>
T* Globals::add_external(T* data) {
    const auto index = index_of<T>();
    DUK_ASSERT(!has<T>());
    if (index >= m_globals.size()) {
        m_globals.resize(index + 1);
    }
    auto& global = m_globals[index] = std::make_unique<ExternalGlobalT<T>>(data);
    return static_cast<T*>(global->get());
}

template<typename T>
void Globals::reset() {
    const auto index = index_of<T>();
    m_globals.at(index).reset();
}

template<typename T>
T* Globals::get() const {
    DUK_ASSERT(has<T>());
    return static_cast<T*>(m_globals[index_of<T>()]->get());
}

template<typename T>
uint64_t Globals::hash_of() {
    static uint64_t hash = std::hash<std::string>()(type_name_of<T>());
    return hash;
}

template<typename T>
uint32_t Globals::index_of() const {
    static uint32_t index = []() -> uint32_t {
        auto hash = hash_of<T>();
        auto& hashToIndex = hash_to_index();
        auto it = hashToIndex.find(hash);
        uint32_t index = hashToIndex.size();
        if (it != hashToIndex.end()) {
            index = it->second;
        } else {
            hashToIndex.emplace(hash, index);
        }
        return index;
    }();
    return index;
}

template<typename T>
bool Globals::has() const {
    const auto index = index_of<T>();
    return index < m_globals.size() && m_globals.at(index);
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
