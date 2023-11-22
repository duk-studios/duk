//
// Created by rov on 11/19/2023.
//

#ifndef DUK_ENGINE_POOL_H
#define DUK_ENGINE_POOL_H

#include <duk_engine/pools/resource.h>

#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>
#include <cassert>

namespace duk::engine {

template<typename T>
class Pool {
public:
    using ResourceHandle = Resource<T>;

    virtual ~Pool();

    void clean();

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD bool empty() const;

    DUK_NO_DISCARD ResourceHandle find(ResourceId id) const;

protected:

    ResourceHandle insert(const std::shared_ptr<T>& resource);

private:
    std::unordered_map<ResourceId, ResourceHandle> m_objects;

};

template<typename T>
Pool<T>::ResourceHandle Pool<T>::find(ResourceId id) const {
    auto it = m_objects.find(id);
    if (it == m_objects.end()) {
        return nullptr;
    }
    return *it;
}

template<typename T>
Pool<T>::~Pool() {
    clean();
    assert(empty() && "some resources are still in use");
}

template<typename T>
void Pool<T>::clean() {
    for (auto it = m_objects.cbegin(); it != m_objects.cend();) {
        const ResourceHandle& ptr = it->second;
        if (ptr.use_count() == 1) {
            m_objects.erase(it++);
            continue;
        }
        ++it;
    }
}

template<typename T>
size_t Pool<T>::size() const {
    return m_objects.size();
}

template<typename T>
bool Pool<T>::empty() const {
    return size() == 0;
}

template<typename T>
Pool<T>::ResourceHandle Pool<T>::insert(const std::shared_ptr<T>& resource) {

    auto id = ResourceId::generate();

    auto [it, inserted] = m_objects.emplace(id, ResourceHandle(id, resource));

    if (!inserted) {
        throw std::runtime_error("failed to insert resource into pool");
    }

    return it->second;
}

}

#endif //DUK_ENGINE_POOL_H
