//
// Created by rov on 11/19/2023.
//

#ifndef DUK_RESOURCE_POOL_H
#define DUK_RESOURCE_POOL_H

#include <duk_resource/resource.h>

#include <cassert>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace duk::resource {

class Pool {
public:
    virtual ~Pool();
    virtual void clean() = 0;
};

template<typename THandle>
class PoolT : public Pool {
public:
    using Type = typename THandle::Type;

    void clean() override;

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD bool empty() const;

    DUK_NO_DISCARD THandle find(Id id) const;

    DUK_NO_DISCARD THandle find_or_default(Id id, const THandle& def) const;

    THandle insert(duk::resource::Id id, const std::shared_ptr<Type>& resource);

private:
    std::unordered_map<Id, THandle> m_objects;
};

class Pools {
public:
    ~Pools();

    template<typename T, typename... Args>
    T* create_pool(Args&&... args)
        requires std::is_base_of_v<PoolT<Handle<typename T::Type>>, T>;

    template<typename T>
    T* get() requires std::is_base_of_v<PoolT<Handle<typename T::Type>>, T>;

    template<typename T>
    const T* get() const requires std::is_base_of_v<PoolT<Handle<typename T::Type>>, T>;

    void clear();

private:
    template<typename T>
    size_t pool_index() const;

private:
    static size_t s_poolIndexCounter;
    std::vector<std::unique_ptr<Pool>> m_pools;
};

template<typename T, typename... Args>
T* Pools::create_pool(Args&&... args)
    requires std::is_base_of_v<PoolT<Handle<typename T::Type>>, T>
{
    const auto index = pool_index<typename T::Type>();
    if (m_pools.size() <= index) {
        m_pools.resize(index + 1);
    }

    auto& pool = m_pools.at(index);
    if (!pool) {
        pool = std::make_unique<T>(std::forward<Args>(args)...);
    }
    return dynamic_cast<T*>(pool.get());
}

template<typename T>
T* Pools::get()
    requires std::is_base_of_v<PoolT<Handle<typename T::Type>>, T>
{
    const auto index = pool_index<typename T::Type>();
    if (index >= m_pools.size()) {
        return nullptr;
    }
    return dynamic_cast<T*>(m_pools.at(index).get());
}

template<typename T>
const T* Pools::get() const requires std::is_base_of_v<PoolT<Handle<typename T::Type>>, T> {
    const auto index = pool_index<typename T::Type>();
    if (index >= m_pools.size()) {
        return nullptr;
    }
    return dynamic_cast<const T*>(m_pools.at(index).get());
}

template<typename T>
size_t Pools::pool_index() const {
    static const auto index = s_poolIndexCounter++;
    return index;
}

template<typename THandle>
THandle PoolT<THandle>::find(Id id) const {
    auto it = m_objects.find(id);
    if (it == m_objects.end()) {
        return THandle(id);
    }
    return it->second;
}

template<typename THandle>
THandle PoolT<THandle>::find_or_default(Id id, const THandle& def) const {
    auto it = find(id);
    if (it.valid()) {
        return it;
    }
    return def;
}

template<typename THandle>
void PoolT<THandle>::clean() {
    for (auto it = m_objects.cbegin(); it != m_objects.cend();) {
        const auto& object = it->second;
        if (object.use_count() == 1) {
            m_objects.erase(it++);
            continue;
        }
        ++it;
    }
}

template<typename THandle>
size_t PoolT<THandle>::size() const {
    return m_objects.size();
}

template<typename THandle>
bool PoolT<THandle>::empty() const {
    return size() == 0;
}

template<typename THandle>
THandle PoolT<THandle>::insert(duk::resource::Id id, const std::shared_ptr<Type>& resource) {
    auto [it, inserted] = m_objects.insert_or_assign(id, THandle(id, resource));

    return it->second;
}

}// namespace duk::resource

#endif//DUK_RESOURCE_POOL_H
