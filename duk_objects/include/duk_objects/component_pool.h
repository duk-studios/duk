/// 06/07/2023
/// component.h

#ifndef DUK_OBJECTS_COMPONENT_H
#define DUK_OBJECTS_COMPONENT_H

#include <duk_macros/macros.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <vector>

namespace duk::scene {

class ComponentPool {
public:
    virtual ~ComponentPool();

    virtual void destruct(uint32_t index) = 0;
};

template<typename T>
class ComponentPoolT : public ComponentPool {
public:
    explicit ComponentPoolT(uint32_t componentsPerChunk);

    ~ComponentPoolT() override;

    template<typename... Args>
    void construct(uint32_t index, Args&&... args);

    void destruct(uint32_t index) override;

    DUK_NO_DISCARD T* get(uint32_t index);

    DUK_NO_DISCARD const T* get(uint32_t index) const;

private:
    uint32_t m_componentsPerChunk;
    mutable std::vector<T*> m_chunks;
};

// Implementations //

template<typename T>
ComponentPoolT<T>::ComponentPoolT(uint32_t componentsPerChunk)
    : m_componentsPerChunk(componentsPerChunk) {
}

template<typename T>
ComponentPoolT<T>::~ComponentPoolT() {
    for (auto& chunk: m_chunks) {
        free(chunk);
    }
}

template<typename T>
template<typename... Args>
void ComponentPoolT<T>::construct(uint32_t index, Args&&... args) {
    auto ptr = get(index);
    ::new (ptr) T(std::forward<Args>(args)...);
}

template<typename T>
void ComponentPoolT<T>::destruct(uint32_t index) {
    auto ptr = get(index);
    ptr->~T();
}

template<typename T>
T* ComponentPoolT<T>::get(uint32_t index) {
    const uint32_t chunkIndex = index / m_componentsPerChunk;
    const uint32_t indexInChunk = index % m_componentsPerChunk;
    if (chunkIndex >= m_chunks.size()) {
        m_chunks.resize(chunkIndex + 1, nullptr);
    }
    auto& chunk = m_chunks[chunkIndex];
    if (!chunk) {
        chunk = (T*)malloc(sizeof(T) * m_componentsPerChunk);
    }
    return chunk + indexInChunk;
}

template<typename T>
const T* ComponentPoolT<T>::get(uint32_t index) const {
    const uint32_t chunkIndex = index / m_componentsPerChunk;
    const uint32_t indexInChunk = index % m_componentsPerChunk;
    if (chunkIndex >= m_chunks.size()) {
        m_chunks.resize(chunkIndex + 1, nullptr);
    }
    auto& chunk = m_chunks[chunkIndex];
    if (!chunk) {
        chunk = (T*)malloc(sizeof(T) * m_componentsPerChunk);
    }
    return m_chunks[chunkIndex] + indexInChunk;
}

}// namespace duk::scene

#endif// DUK_OBJECTS_COMPONENT_H
