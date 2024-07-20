//
// Created by rov on 11/20/2023.
//

#ifndef DUK_RESOURCE_RESOURCE_H
#define DUK_RESOURCE_RESOURCE_H

#include <duk_macros/macros.h>
#include <duk_serial/json/types.h>

#include <memory>
#include <typeindex>
#include <vector>

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

// 1,000,000 reserved for built-in resources
static constexpr Id kMaxBuiltInResourceId(1000000);

template<typename T>
class HandleAccess {
public:
    DUK_NO_DISCARD T* operator->() const;

    DUK_NO_DISCARD T& operator*() const;
};

template<>
class HandleAccess<void> {
    // void handles have no access operators
};

template<typename T>
class Handle : public HandleAccess<T> {
public:
    using Type = T;

    Handle();

    Handle(Id id);

    Handle(Id id, const std::shared_ptr<T>& resource);

    Handle(const std::shared_ptr<T>& resource);

    template<typename U>
    Handle(const Handle<U>& other)
        requires std::is_convertible_v<U*, T*>;

    template<typename U>
    Handle(Id id, const std::shared_ptr<U>& resource)
        requires std::is_convertible_v<U*, T*>;

    template<typename U>
    Handle(const std::shared_ptr<U>& resource)
        requires std::is_convertible_v<U*, T*>;

    template<typename U>
    Handle<U> as() const;

    Id id() const;

    T* get() const;

    bool valid() const;

    operator bool() const;

    size_t use_count() const;

    void reset(Id id = kInvalidId);

private:
    template<typename U>
    friend class Handle;

    Id m_id;
    std::shared_ptr<T> m_resource;
};

template<typename T>
T* HandleAccess<T>::operator->() const {
    return static_cast<const Handle<T>*>(this)->get();
}

template<typename T>
T& HandleAccess<T>::operator*() const {
    return *static_cast<const Handle<T>*>(this)->get();
}

template<typename T>
Handle<T>::Handle()
    : Handle(kInvalidId, nullptr) {
}

template<typename T>
Handle<T>::Handle(Id id)
    : Handle(id, nullptr) {
}

template<typename T>
Handle<T>::Handle(const Id id, const std::shared_ptr<T>& resource)
    : m_id(id)
    , m_resource(resource) {
}

template<typename T>
Handle<T>::Handle(const std::shared_ptr<T>& resource)
    : Handle(kInvalidId, resource) {
}

template<typename T>
template<typename U>
Handle<T>::Handle(const Handle<U>& other)
    requires std::is_convertible_v<U*, T*>
    : Handle(other.id(), other.m_resource) {
}

template<typename T>
template<typename U>
Handle<T>::Handle(Id id, const std::shared_ptr<U>& resource)
    requires std::is_convertible_v<U*, T*>
    : m_id(id)
    , m_resource(resource) {
}

template<typename T>
template<typename U>
Handle<T>::Handle(const std::shared_ptr<U>& resource)
    requires std::is_convertible_v<U*, T*>
    : m_id(kInvalidId)
    , m_resource(resource) {
}

template<typename T>
template<typename U>
Handle<U> Handle<T>::as() const {
    return Handle<U>(m_id, std::static_pointer_cast<U>(m_resource));
}

template<typename T>
Id Handle<T>::id() const {
    return m_id;
}

template<typename T>
void Handle<T>::reset(const Id id) {
    m_id = id;
    m_resource.reset();
}

template<typename T>
size_t Handle<T>::use_count() const {
    return m_resource.use_count();
}

template<typename T>
bool Handle<T>::valid() const {
    return m_resource != nullptr;
}

template<typename T>
Handle<T>::operator bool() const {
    return valid();
}

template<typename T>
T* Handle<T>::get() const {
    return reinterpret_cast<T*>(m_resource.get());
}

}// namespace duk::resource

namespace duk::serial {

template<>
inline void from_json<duk::resource::Id>(const rapidjson::Value& jsonObject, duk::resource::Id& id) {
    id = duk::resource::Id(jsonObject.Get<uint64_t>());
}

template<>
inline void to_json<duk::resource::Id>(rapidjson::Document& document, rapidjson::Value& json, const duk::resource::Id& id) {
    json.Set(id.value());
}

template<typename T>
void from_json(const rapidjson::Value& jsonObject, duk::resource::Handle<T>& resource) {
    resource.reset(duk::resource::Id(jsonObject.Get<uint64_t>()));
}

template<typename T>
void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::resource::Handle<T>& resource) {
    json.Set<uint64_t>(resource.id().value());
}

template<typename T>
void from_json(const rapidjson::Value& jsonObject, std::vector<duk::resource::Handle<T>>& resources) {
    DUK_ASSERT(jsonObject.IsArray());
    auto jsonArray = jsonObject.GetArray();
    for (auto& jsonElement: jsonArray) {
        from_json(jsonElement, resources.emplace_back());
    }
}

template<typename T>
void to_json(rapidjson::Document& document, rapidjson::Value& json, const std::vector<duk::resource::Handle<T>>& resources) {
    auto jsonArray = json.SetArray().GetArray();
    for (const auto& element: resources) {
        rapidjson::Value jsonElement;
        to_json(document, jsonElement, element);
        jsonArray.PushBack(jsonElement, document.GetAllocator());
    }
}

}// namespace duk::serial

template<>
struct std::hash<duk::resource::Id> {
    size_t operator()(const duk::resource::Id& resourceId) const noexcept {
        return resourceId.value();
    }
};

#endif//DUK_RESOURCE_RESOURCE_H
