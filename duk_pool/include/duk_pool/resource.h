//
// Created by rov on 11/20/2023.
//

#ifndef DUK_POOL_RESOURCE_H
#define DUK_POOL_RESOURCE_H

#include <duk_macros/macros.h>
#include <duk_hash/hash_combine.h>

#include <memory>

namespace duk::pool {

class ResourceId {
public:

    static ResourceId generate();

    ResourceId();

    explicit ResourceId(uint64_t id);

    DUK_NO_DISCARD bool operator==(const ResourceId& rhs) const;

    DUK_NO_DISCARD bool operator!=(const ResourceId& rhs) const;

    DUK_NO_DISCARD uint64_t value() const;

private:
    uint64_t m_id;
};

template<typename T>
class Resource {
public:

    using Type = T;

    Resource();

    Resource(ResourceId id, const std::shared_ptr<T>& resource);

    template<typename U>
    Resource(const Resource<U>& other) requires std::is_base_of_v<T, U>;

    template<typename U>
    Resource<U> as() requires std::is_base_of_v<T, U>;

    void reset();

    DUK_NO_DISCARD T* get();

    DUK_NO_DISCARD T* get() const;

    DUK_NO_DISCARD T* operator->();

    DUK_NO_DISCARD T* operator->() const;

    DUK_NO_DISCARD T& operator*();

    DUK_NO_DISCARD T& operator*() const;

    DUK_NO_DISCARD ResourceId id() const;

    DUK_NO_DISCARD size_t use_count() const;

    DUK_NO_DISCARD bool valid() const;

private:
    template<typename U>
    friend class Resource;
    ResourceId m_id;
    std::shared_ptr<T> m_resource;
};

template<typename T>
Resource<T>::Resource() :
    Resource(ResourceId(0), nullptr) {

}

template<typename T>
Resource<T>::Resource(ResourceId id, const std::shared_ptr<T>& resource) :
        m_id(id),
        m_resource(resource) {

}

template<typename T>
template<typename U>
Resource<T>::Resource(const Resource<U>& other) requires std::is_base_of_v<T, U> {
    m_resource = other.m_resource;
    m_id = other.m_id;
}

template<typename T>
template<typename U>
Resource<U> Resource<T>::as() requires std::is_base_of_v<T, U> {
    return Resource<U>(m_id, std::dynamic_pointer_cast<U>(m_resource));
}

template<typename T>
void Resource<T>::reset() {
    m_resource.reset();
    m_id = ResourceId(0);
}

template<typename T>
T* Resource<T>::get() {
    return m_resource.get();
}

template<typename T>
T* Resource<T>::get() const {
    return m_resource.get();
}

template<typename T>
T* Resource<T>::operator->() {
    return m_resource.get();
}

template<typename T>
T* Resource<T>::operator->() const {
    return m_resource.get();
}

template<typename T>
T& Resource<T>::operator*() {
    return *m_resource;
}

template<typename T>
T& Resource<T>::operator*() const {
    return *m_resource;
}

template<typename T>
ResourceId Resource<T>::id() const {
    return m_id;
}

template<typename T>
size_t Resource<T>::use_count() const {
    return m_resource.use_count();
}

template<typename T>
bool Resource<T>::valid() const {
    return m_resource && m_id.value() != 0;
}

}

namespace std {

template<>
struct hash<duk::pool::ResourceId> {
    size_t operator()(const duk::pool::ResourceId& resourceId) const {
        return resourceId.value();
    }
};

}

#endif //DUK_POOL_RESOURCE_H
