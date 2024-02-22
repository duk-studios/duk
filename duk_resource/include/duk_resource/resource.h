//
// Created by rov on 11/20/2023.
//

#ifndef DUK_RESOURCE_RESOURCE_H
#define DUK_RESOURCE_RESOURCE_H

#include <duk_macros/macros.h>
#include <duk_json/types.h>

#include <memory>
#include <typeindex>

namespace duk::resource {

class Id {
public:

    constexpr Id() : Id(0) {}

    explicit constexpr Id(uint64_t id) : m_id(id) {}

    DUK_NO_DISCARD bool operator==(const Id& rhs) const;

    DUK_NO_DISCARD bool operator!=(const Id& rhs) const;

    DUK_NO_DISCARD bool operator<(const Id& rhs) const;

    DUK_NO_DISCARD uint64_t value() const;

private:
    uint64_t m_id;
};

template<typename T>
class ResourceT;

class Resource {
public:

    DUK_NO_DISCARD Id id() const;

    template<typename T>
    DUK_NO_DISCARD ResourceT<T> as();

    template<typename T>
    DUK_NO_DISCARD bool is() const;

    DUK_NO_DISCARD size_t use_count() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD operator bool() const;

    void reset();

protected:

    template<typename T>
    Resource(Id id, const std::shared_ptr<T>& resource);

    template<typename U>
    friend class ResourceT;

    Id m_id;
    std::shared_ptr<void> m_resource;
    // used for safe casting
    std::type_index m_type;
};

template<typename T>
class ResourceT : public Resource {
public:

    using Type = T;

    ResourceT();

    ResourceT(Id id);

    ResourceT(Id id, const std::shared_ptr<T>& resource);

    template<typename U>
    ResourceT(const ResourceT<U>& other) requires std::is_base_of_v<T, U>;

    DUK_NO_DISCARD T* get();

    DUK_NO_DISCARD T* get() const;

    DUK_NO_DISCARD T* operator->();

    DUK_NO_DISCARD T* operator->() const;

    DUK_NO_DISCARD T& operator*();

    DUK_NO_DISCARD T& operator*() const;

};

template<typename T>
Resource::Resource(Id id, const std::shared_ptr<T>& resource) :
    m_id(id),
    m_resource(resource),
    m_type(typeid(T)) {

}

template<typename T>
ResourceT<T> Resource::as() {
    return ResourceT<T>(m_id, std::reinterpret_pointer_cast<T>(m_resource));
}

template<typename T>
bool Resource::is() const {
    return m_type == typeid(T);
}

template<typename T>
ResourceT<T>::ResourceT() :
    ResourceT(Id(0), nullptr) {

}

template<typename T>
ResourceT<T>::ResourceT(Id id) :
    Resource(id, std::shared_ptr<T>()) {

}

template<typename T>
ResourceT<T>::ResourceT(Id id, const std::shared_ptr<T>& resource) :
    Resource(id, resource) {

}

template<typename T>
template<typename U>
ResourceT<T>::ResourceT(const ResourceT<U>& other) requires std::is_base_of_v<T, U> :
    Resource(other.id(), other.m_resource) {

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

}

namespace duk::json {

template<>
inline void from_json<duk::resource::Id>(const rapidjson::Value& jsonObject, duk::resource::Id& object) {
    object = duk::resource::Id(from_json<uint64_t>(jsonObject));
}

}

namespace std {

template<>
struct hash<duk::resource::Id> {
    size_t operator()(const duk::resource::Id& resourceId) const {
        return resourceId.value();
    }
};

}

#endif //DUK_RESOURCE_RESOURCE_H
