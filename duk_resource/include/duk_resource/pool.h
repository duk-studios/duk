//
// Created by rov on 11/19/2023.
//

#ifndef DUK_RESOURCE_POOL_H
#define DUK_RESOURCE_POOL_H

#include <duk_resource/resource.h>

#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>
#include <cassert>

namespace duk::resource {

class Pool {
public:

    virtual ~Pool();

};

template<typename ResourceT>
class PoolT : public Pool {
public:

    using ResourceType = typename ResourceT::Type;

    ~PoolT() override;

    void clean();

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD bool empty() const ;

    DUK_NO_DISCARD ResourceT find(Id id) const;

    DUK_NO_DISCARD ResourceT find_or_default(Id id, const ResourceT& def) const;

protected:

    ResourceT insert(duk::resource::Id id, const std::shared_ptr<ResourceType>& resource);

private:
    std::unordered_map<Id, ResourceT> m_objects;

};

template<typename ResourceT>
ResourceT PoolT<ResourceT>::find(Id id) const {
    auto it = m_objects.find(id);
    if (it == m_objects.end()) {
        return ResourceT(id);
    }
    return it->second;
}

template<typename ResourceT>
ResourceT PoolT<ResourceT>::find_or_default(Id id, const ResourceT& def) const {
    auto it = find(id);
    if (it.valid()) {
        return it;
    }
    return def;
}

template<typename ResourceT>
PoolT<ResourceT>::~PoolT() {
    clean();
    assert(empty() && "some resources are still in use");
}

template<typename ResourceT>
void PoolT<ResourceT>::clean() {
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
size_t PoolT<ResourceT>::size() const {
    return m_objects.size();
}

template<typename ResourceT>
bool PoolT<ResourceT>::empty() const {
    return size() == 0;
}

template<typename ResourceT>
ResourceT PoolT<ResourceT>::insert(duk::resource::Id id, const std::shared_ptr<ResourceType>& resource) {

    auto [it, inserted] = m_objects.emplace(id, ResourceT(id, resource));

    if (!inserted) {
        throw std::runtime_error("failed to insert resource into pool");
    }

    return it->second;
}

}

#endif //DUK_RESOURCE_POOL_H
