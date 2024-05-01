//
// Created by Ricardo on 18/04/2024.
//

#ifndef DUK_SERIAL_JSON_TYPES_H
#define DUK_SERIAL_JSON_TYPES_H

#include <duk_serial/json/rapidjson_import.h>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <set>
#include <vector>
#include <sstream>

namespace duk::serial {

template<typename T>
void from_json(const rapidjson::Value& json, T& value) {
    value = json.Get<T>();
}

template<typename T>
void to_json(rapidjson::Document& document, rapidjson::Value& json, const T& value) {
    json.Set(value, document.GetAllocator());
}

template<>
inline void from_json<std::string>(const rapidjson::Value& json, std::string& value) {
    value = json.GetString();
}

template<>
inline void to_json<std::string>(rapidjson::Document& document, rapidjson::Value& json, const std::string& value) {
    json.SetString(value.c_str(), document.GetAllocator());
}

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

template<>
inline void from_json<glm::vec2>(const rapidjson::Value& jsonObject, glm::vec2& vec) {
    vec = detail::parse_vec<glm::vec2>(jsonObject);
}

template<>
inline void to_json<glm::vec2>(rapidjson::Document& document, rapidjson::Value& json, const glm::vec2& vec) {
    auto vecStr = fmt::format("{0};{1}", vec.x, vec.y);
    json.SetString(vecStr.c_str(), vecStr.size(), document.GetAllocator());
}

template<>
inline void from_json<glm::ivec2>(const rapidjson::Value& jsonObject, glm::ivec2& vec) {
    vec = detail::parse_vec<glm::ivec2>(jsonObject);
}

template<>
inline void to_json<glm::ivec2>(rapidjson::Document& document, rapidjson::Value& json, const glm::ivec2& vec) {
    auto vecStr = fmt::format("{0};{1}", vec.x, vec.y);
    json.SetString(vecStr.c_str(), vecStr.size(), document.GetAllocator());
}

template<>
inline void from_json<glm::vec3>(const rapidjson::Value& jsonObject, glm::vec3& vec) {
    vec = detail::parse_vec<glm::vec3>(jsonObject);
}

template<>
inline void to_json<glm::vec3>(rapidjson::Document& document, rapidjson::Value& json, const glm::vec3& vec) {
    auto vecStr = fmt::format("{0};{1};{2}", vec.x, vec.y, vec.z);
    json.SetString(vecStr.c_str(), vecStr.size(), document.GetAllocator());
}

template<>
inline void from_json<glm::ivec3>(const rapidjson::Value& jsonObject, glm::ivec3& vec) {
    vec = detail::parse_vec<glm::ivec3>(jsonObject);
}

template<>
inline void to_json<glm::ivec3>(rapidjson::Document& document, rapidjson::Value& json, const glm::ivec3& vec) {
    auto vecStr = fmt::format("{0};{1};{2}", vec.x, vec.y, vec.z);
    json.SetString(vecStr.c_str(), vecStr.size(), document.GetAllocator());
}

template<>
inline void from_json<glm::vec4>(const rapidjson::Value& jsonObject, glm::vec4& vec) {
    vec = detail::parse_vec<glm::vec4>(jsonObject);
}

template<>
inline void to_json<glm::vec4>(rapidjson::Document& document, rapidjson::Value& json, const glm::vec4& vec) {
    auto vecStr = fmt::format("{0};{1};{2};{3}", vec.x, vec.y, vec.z, vec.w);
    json.SetString(vecStr.c_str(), vecStr.size(), document.GetAllocator());
}

template<>
inline void from_json<glm::ivec4>(const rapidjson::Value& jsonObject, glm::ivec4& vec) {
    vec = detail::parse_vec<glm::ivec4>(jsonObject);
}

template<>
inline void to_json<glm::ivec4>(rapidjson::Document& document, rapidjson::Value& json, const glm::ivec4& vec) {
    auto vecStr = fmt::format("{0};{1};{2};{3}", vec.x, vec.y, vec.z, vec.w);
    json.SetString(vecStr.c_str(), vecStr.size(), document.GetAllocator());
}

template<>
inline void from_json<glm::quat>(const rapidjson::Value& jsonObject, glm::quat& quat) {
    quat = glm::quat(glm::radians(detail::parse_vec<glm::vec3>(jsonObject)));
}

template<>
inline void to_json<glm::quat>(rapidjson::Document& document, rapidjson::Value& json, const glm::quat& quat) {
    to_json<glm::vec3>(document, json, glm::eulerAngles(quat));
}

template<typename T>
void from_json(const rapidjson::Value& jsonObject, std::set<T>& set) {
    DUK_ASSERT(jsonObject.IsArray());
    auto jsonArray = jsonObject.GetArray();
    for (auto& jsonElement: jsonArray) {
        T element = {};
        from_json(jsonElement, element);
        set.insert(element);
    }
}

template<typename T>
void to_json(rapidjson::Document& document, rapidjson::Value& json, const std::set<T>& set) {
    auto jsonArray = json.SetArray().GetArray();
    for (const auto& element: set) {
        rapidjson::Value jsonElement;
        to_json(document, jsonElement, element);
        jsonArray.PushBack(jsonElement, document.GetAllocator());
    }
}

template<typename T>
void from_json(const rapidjson::Value& jsonObject, std::vector<T>& vector) {
    DUK_ASSERT(jsonObject.IsArray());
    auto jsonArray = jsonObject.GetArray();
    for (auto& jsonElement: jsonArray) {
        from_json(jsonElement, vector.emplace_back());
    }
}

template<typename T>
void to_json(rapidjson::Document& document, rapidjson::Value& json, const std::vector<T>& vector) {
    auto jsonArray = json.SetArray().GetArray();
    for (const auto& element: vector) {
        rapidjson::Value jsonElement;
        to_json<T>(document, jsonElement, element);
        jsonArray.PushBack(jsonElement, document.GetAllocator());
    }
}

// these are defined in json/serializer.h
template<typename T>
void from_json_member(const rapidjson::Value& json, const char* name, T& value, bool optional = false);

template<typename T>
void to_json_member(rapidjson::Document& document, rapidjson::Value& json, const char* name, const T& value);

}// namespace duk::serial

#endif//DUK_SERIAL_JSON_TYPES_H
