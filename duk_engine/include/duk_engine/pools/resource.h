//
// Created by rov on 11/20/2023.
//

#ifndef DUK_ENGINE_RESOURCE_H
#define DUK_ENGINE_RESOURCE_H

#include <duk_macros/macros.h>
#include <duk_hash/hash_combine.h>

#include <memory>

namespace duk::engine {

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

    Resource(ResourceId id, const std::shared_ptr<T>& resource);

    DUK_NO_DISCARD T* get();

    DUK_NO_DISCARD T* get() const;

    DUK_NO_DISCARD T* operator->();

    DUK_NO_DISCARD T* operator->() const;

    DUK_NO_DISCARD T& operator*();

    DUK_NO_DISCARD T& operator*() const;

    DUK_NO_DISCARD ResourceId id() const;

    DUK_NO_DISCARD size_t use_count() const;

private:
    ResourceId m_id;
    std::shared_ptr<T> m_resource;
};

template<typename T>
Resource<T>::Resource(ResourceId id, const std::shared_ptr<T>& resource) :
        m_id(id),
        m_resource(resource) {

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

}

namespace std {

template<>
struct hash<duk::engine::ResourceId> {
    size_t operator()(const duk::engine::ResourceId& resourceId) const {
        return resourceId.value();
    }
};

}

#endif //DUK_ENGINE_RESOURCE_H
