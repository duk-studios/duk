//
// Created by rov on 11/19/2023.
//

#ifndef DUK_RESOURCE_POOL_H
#define DUK_RESOURCE_POOL_H

#include <duk_resource/handle.h>

#include <duk_log/log.h>

#include <duk_tools/types.h>

#include <unordered_map>
#include <vector>

namespace duk::resource {

class Pool {
public:
    virtual ~Pool();

    virtual void clean() = 0;

    virtual bool contains(Id id) const = 0;
};

template<typename T>
class PoolT final : public Pool {
public:
    void clean() override;

    DUK_NO_DISCARD bool contains(Id id) const override;

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD bool empty() const;

    DUK_NO_DISCARD Handle<T> find(Id id) const;

    DUK_NO_DISCARD Handle<T> find_or_default(Id id, const Handle<T>& def) const;

    Handle<T> insert(duk::resource::Id id, const std::shared_ptr<T>& resource);

private:
    std::unordered_map<Id, Handle<T>> m_resources;
};

class Pools {
public:
    ~Pools();

    template<typename T>
    DUK_NO_DISCARD PoolT<T>* get();

    template<typename T>
    Handle<T> insert(Id id, const std::shared_ptr<T>& resource);

    template<typename T>
    DUK_NO_DISCARD Handle<T> find(Id id) const;

    DUK_NO_DISCARD bool contains(Id id) const;

    void clear();

private:
    static std::unordered_map<std::string, size_t>& pool_type_name_to_index();

    template<typename T>
    size_t pool_index() const;

private:
    std::vector<std::unique_ptr<Pool>> m_pools;
};

template<typename T>
PoolT<T>* Pools::get() {
    const auto index = pool_index<T>();
    if (index >= m_pools.size()) {
        m_pools.resize(index + 1);
    }

    auto& pool = m_pools.at(index);
    if (!pool) {
        pool = std::make_unique<PoolT<T>>();
    }

    return static_cast<PoolT<T>*>(pool.get());
}

template<typename T>
Handle<T> Pools::insert(Id id, const std::shared_ptr<T>& resource) {
    auto pool = get<T>();
    return pool->insert(id, resource);
}

template<typename T>
Handle<T> Pools::find(Id id) const {
    auto pool = get<T>();
    return pool->find(id);
}

template<typename T>
size_t Pools::pool_index() const {
    static const auto index = [] {
        auto& typeNameToIndex = pool_type_name_to_index();
        const auto& typeName = duk::tools::type_name_of<T>();
        auto it = typeNameToIndex.find(typeName);
        if (it != typeNameToIndex.end()) {
            return it->second;
        }
        size_t index = typeNameToIndex.size();
        typeNameToIndex[typeName] = index;
        return index;
    }();
    return index;
}

template<typename T>
Handle<T> PoolT<T>::find(Id id) const {
    auto it = m_resources.find(id);
    if (it == m_resources.end()) {
        return Handle<T>(id);
    }
    return it->second;
}

template<typename T>
Handle<T> PoolT<T>::find_or_default(Id id, const Handle<T>& def) const {
    auto it = find(id);
    if (it.valid()) {
        return it;
    }
    return def;
}

template<typename T>
void PoolT<T>::clean() {
    for (auto it = m_resources.cbegin(); it != m_resources.cend();) {
        const auto& object = it->second;
        if (object.use_count() == 1) {
            m_resources.erase(it++);
            continue;
        }
        ++it;
    }
}

template<typename T>
bool PoolT<T>::contains(Id id) const {
    return m_resources.contains(id);
}

template<typename T>
size_t PoolT<T>::size() const {
    return m_resources.size();
}

template<typename T>
bool PoolT<T>::empty() const {
    return size() == 0;
}

template<typename T>
Handle<T> PoolT<T>::insert(duk::resource::Id id, const std::shared_ptr<T>& resource) {
    auto [it, inserted] = m_resources.insert_or_assign(id, Handle<T>(id, resource));
    if (!inserted) {
        duk::log::info("Resource with id {} already exists, replacing", id.value());
    }

    return it->second;
}

}// namespace duk::resource

#endif//DUK_RESOURCE_POOL_H
