//
// Created by rov on 11/19/2023.
//

#ifndef DUK_POOL_POOL_H
#define DUK_POOL_POOL_H

#include <duk_pool/resource.h>

#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>
#include <cassert>

namespace duk::pool {

template<typename ResourceT>
class Pool {
public:

    using ResourceType = typename ResourceT::Type;

    virtual ~Pool();

    void clean();

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD bool empty() const;

    DUK_NO_DISCARD ResourceT find(ResourceId id) const;

protected:

    ResourceT insert(const std::shared_ptr<ResourceType>& resource);

private:
    std::unordered_map<ResourceId, ResourceT> m_objects;

};

template<typename ResourceT>
ResourceT Pool<ResourceT>::find(ResourceId id) const {
    auto it = m_objects.find(id);
    if (it == m_objects.end()) {
        return nullptr;
    }
    return *it;
}

template<typename ResourceT>
Pool<ResourceT>::~Pool() {
    clean();
    assert(empty() && "some resources are still in use");
}

template<typename ResourceT>
void Pool<ResourceT>::clean() {
    for (auto it = m_objects.cbegin(); it != m_objects.cend();) {
        const auto& object = it->second;
        if (object.use_count() == 1) {
            m_objects.erase(it++);
            continue;
        }
        ++it;
    }
}

template<typename ResourceT>
size_t Pool<ResourceT>::size() const {
    return m_objects.size();
}

template<typename ResourceT>
bool Pool<ResourceT>::empty() const {
    return size() == 0;
}

template<typename ResourceT>
ResourceT Pool<ResourceT>::insert(const std::shared_ptr<ResourceType>& resource) {

    auto id = ResourceId::generate();

    auto [it, inserted] = m_objects.emplace(id, ResourceT(id, resource));

    if (!inserted) {
        throw std::runtime_error("failed to insert resource into pool");
    }

    return it->second;
}

}

#endif //DUK_POOL_POOL_H
