//
// Created by rov on 11/20/2023.
//

#ifndef DUK_RESOURCE_RESOURCE_H
#define DUK_RESOURCE_RESOURCE_H

#include <duk_macros/macros.h>
#include <duk_serial/json_serializer.h>

#include <memory>
#include <typeindex>

namespace duk::resource {

class Id {
public:
    constexpr Id()
        : Id(0) {
    }

    explicit constexpr Id(uint64_t id)
        : m_id(id) {
    }

    DUK_NO_DISCARD bool operator==(const Id& rhs) const;

    DUK_NO_DISCARD bool operator!=(const Id& rhs) const;

    DUK_NO_DISCARD bool operator<(const Id& rhs) const;

    DUK_NO_DISCARD uint64_t value() const;

    DUK_NO_DISCARD bool valid() const;

private:
    uint64_t m_id;
};

static constexpr duk::resource::Id kInvalidId;

class Resource {
public:
    virtual ~Resource();

    DUK_NO_DISCARD Id id() const;

    virtual void reset(Id id = kInvalidId);

    DUK_NO_DISCARD virtual size_t use_count() const = 0;

    DUK_NO_DISCARD virtual bool valid() const = 0;

    DUK_NO_DISCARD virtual operator bool() const = 0;

protected:
    Resource(Id id);

protected:
    Id m_id;
};

template<typename T>
class ResourceT : public Resource {
public:
    using Type = T;

    ResourceT();

    ResourceT(Id id);

    ResourceT(Id id, const std::shared_ptr<T>& resource);

    ResourceT(const std::shared_ptr<T>& resource);

    template<typename U>
    ResourceT(const ResourceT<U>& other)
        requires std::is_base_of_v<T, U>;

    void reset(Id id = kInvalidId) override;

    size_t use_count() const override;

    bool valid() const override;

    operator bool() const override;

    DUK_NO_DISCARD T* get();

    DUK_NO_DISCARD T* get() const;

    DUK_NO_DISCARD T* operator->();

    DUK_NO_DISCARD T* operator->() const;

    DUK_NO_DISCARD T& operator*();

    DUK_NO_DISCARD T& operator*() const;

private:
    template<typename U>
    ResourceT(Id id, const std::shared_ptr<U>& resource);

    template<typename U>
    friend class ResourceT;
    std::shared_ptr<T> m_resource;
};

template<typename T>
ResourceT<T>::ResourceT()
    : ResourceT(kInvalidId, nullptr) {
}

template<typename T>
ResourceT<T>::ResourceT(Id id)
    : ResourceT(id, std::shared_ptr<T>()) {
}

template<typename T>
ResourceT<T>::ResourceT(Id id, const std::shared_ptr<T>& resource)
    : Resource(id)
    , m_resource(resource) {
}

template<typename T>
ResourceT<T>::ResourceT(const std::shared_ptr<T>& resource)
    : Resource(kInvalidId)
    , m_resource(resource) {
}

template<typename T>
template<typename U>
ResourceT<T>::ResourceT(const ResourceT<U>& other)
    requires std::is_base_of_v<T, U>
    : Resource(other.id(), other.m_resource) {
}

template<typename T>
void ResourceT<T>::reset(Id id) {
    Resource::reset(id);
    m_resource.reset();
}

template<typename T>
size_t ResourceT<T>::use_count() const {
    return m_resource.use_count();
}

template<typename T>
bool ResourceT<T>::valid() const {
    return m_resource && m_id.value() != 0;
}

template<typename T>
ResourceT<T>::operator bool() const {
    return valid();
}

template<typename T>
T* ResourceT<T>::get() {
    return reinterpret_cast<T*>(m_resource.get());
}

template<typename T>
T* ResourceT<T>::get() const {
    return reinterpret_cast<T*>(m_resource.get());
}

template<typename T>
T* ResourceT<T>::operator->() {
    return get();
}

template<typename T>
T* ResourceT<T>::operator->() const {
    return get();
}

template<typename T>
T& ResourceT<T>::operator*() {
    return *get();
}

template<typename T>
T& ResourceT<T>::operator*() const {
    return *get();
}

}// namespace duk::resource

namespace duk::serial {

template<>
inline void from_json<duk::resource::Id>(const rapidjson::Value& jsonObject, duk::resource::Id& id) {
    id = duk::resource::Id(jsonObject.Get<uint64_t>());
}

template<>
inline void to_json<duk::resource::Id>(const duk::resource::Id& id, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    jsonObject.Set(id.value());
}

template<>
inline void from_json<duk::resource::Resource>(const rapidjson::Value& jsonObject, duk::resource::Resource& resource) {
    resource.reset(duk::resource::Id(jsonObject.Get<uint64_t>()));
}

template<>
inline void to_json<duk::resource::Resource>(const duk::resource::Resource& resource, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    jsonObject.Set<uint64_t>(resource.id().value());
}

}// namespace duk::serial

namespace std {

template<>
struct hash<duk::resource::Id> {
    size_t operator()(const duk::resource::Id& resourceId) const {
        return resourceId.value();
    }
};

}// namespace std

#endif//DUK_RESOURCE_RESOURCE_H
