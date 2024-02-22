/// 06/07/2023
/// component.h

#ifndef DUK_SCENE_COMPONENT_H
#define DUK_SCENE_COMPONENT_H

#include <duk_macros/macros.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace duk::scene {

class ComponentPool {
public:
    virtual ~ComponentPool();

    virtual void destruct(uint32_t index) = 0;
};

template<typename T>
class ComponentPoolT : public ComponentPool {
public:
    explicit ComponentPoolT(uint32_t componentCount);

    ~ComponentPoolT() override;

    template<typename... Args>
    void construct(uint32_t index, Args&&... args);

    void destruct(uint32_t index) override;

    DUK_NO_DISCARD T* get(uint32_t index);

    DUK_NO_DISCARD const T* get(uint32_t index) const;

private:
    uint32_t m_componentCount;
    T* m_components;
    uint32_t m_allocationCount;
};

// Implementations //

template<typename T>
ComponentPoolT<T>::ComponentPoolT(uint32_t componentCount) : m_componentCount(componentCount),
                                                             m_components((T*)malloc(componentCount * sizeof(T))),
                                                             m_allocationCount(0) {
}

template<typename T>
ComponentPoolT<T>::~ComponentPoolT() {
    assert(m_allocationCount == 0);
    free(m_components);
}

template<typename T>
template<typename... Args>
void ComponentPoolT<T>::construct(uint32_t index, Args&&... args) {
    auto ptr = get(index);
    ::new (ptr) T(std::forward<Args>(args)...);
    m_allocationCount++;
}

template<typename T>
void ComponentPoolT<T>::destruct(uint32_t index) {
    auto ptr = get(index);
    ptr->~T();
    m_allocationCount--;
}

template<typename T>
T* ComponentPoolT<T>::get(uint32_t index) {
    assert(index < m_componentCount);
    return m_components + index;
}

template<typename T>
const T* ComponentPoolT<T>::get(uint32_t index) const {
    assert(index < m_componentCount);
    return m_components + index;
}

}// namespace duk::scene

#endif// DUK_SCENE_COMPONENT_H
