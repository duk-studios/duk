//
// Created by rov on 12/5/2023.
//

#ifndef DUK_JSON_TYPES_H
#define DUK_JSON_TYPES_H

#include <glm/glm.hpp>
#include <rapidjson/document.h>
#include <sstream>

namespace duk::json {

namespace detail {

template<typename VecT>
static VecT parse_vec(const rapidjson::Value& member) {
    std::stringstream str(member.GetString());
    std::string segment;
    VecT result = {};
    for (int i = 0; std::getline(str, segment, ';') && i < VecT::length(); i++) {
        result[i] = std::stof(segment);
    }
    return result;
}

}// namespace detail

// specialize this method for custom types
template<typename T>
void from_json(const rapidjson::Value& jsonObject, T& object) {
    object = jsonObject.Get<T>();
}

template<typename T>
T from_json(const rapidjson::Value& jsonObject) {
    T object = {};
    from_json<T>(jsonObject, object);
    return object;
}

template<typename T>
std::unique_ptr<T> make_unique_from_json(const rapidjson::Value& jsonObject) {
    return std::make_unique<T>(from_json<T>(jsonObject));
}

template<typename T>
std::shared_ptr<T> make_shared_from_json(const rapidjson::Value& jsonObject) {
    return std::make_shared<T>(from_json<T>(jsonObject));
}

template<typename T>
T from_json_member(const rapidjson::Value& jsonObject, const char* memberName, const T& defaultValue = {}) {
    auto it = jsonObject.FindMember(memberName);
    if (it == jsonObject.MemberEnd()) {
        return defaultValue;
    }
    return from_json<T>(it->value);
}

template<typename T>
void from_json_member(const rapidjson::Value& jsonObject, const char* memberName, T& value) {
    auto it = jsonObject.FindMember(memberName);
    if (it == jsonObject.MemberEnd()) {
        return;
    }
    from_json<T>(it->value, value);
}

template<typename T>
std::unique_ptr<T> make_unique_from_json_member(const rapidjson::Value& jsonObject, const char* memberName, const T& defaultValue) {
    return std::make_unique<T>(from_json_member<T>(jsonObject, memberName, defaultValue));
}

template<typename T>
std::shared_ptr<T> make_shared_from_json(const rapidjson::Value& jsonObject, const char* memberName, const T& defaultValue) {
    return std::make_shared<T>(from_json_member<T>(jsonObject, memberName, defaultValue));
}

template<>
inline void from_json<glm::vec2>(const rapidjson::Value& jsonObject, glm::vec2& object) {
    object = detail::parse_vec<glm::vec2>(jsonObject);
}

template<>
inline void from_json<glm::vec3>(const rapidjson::Value& jsonObject, glm::vec3& object) {
    object = detail::parse_vec<glm::vec3>(jsonObject);
}

template<>
inline void from_json<glm::vec4>(const rapidjson::Value& jsonObject, glm::vec4& object) {
    object = detail::parse_vec<glm::vec4>(jsonObject);
}

}// namespace duk::json

#endif//DUK_JSON_TYPES_H
