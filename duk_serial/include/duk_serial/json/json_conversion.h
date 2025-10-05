//
// Created by rov on 10/5/2025.
//

#ifndef DUK_SERIAL_JSON_CONVERSION_H
#define DUK_SERIAL_JSON_CONVERSION_H

#include <duk_serial/json/rapidjson_import.h>

#include <fmt/format.h>
#include <duk_math/math.h>

#include <set>
#include <vector>
#include <sstream>
#include <filesystem>

namespace duk::serial {

template<typename T>
void from_json(const rapidjson::Value& json, T& value) {
    value = json.Get<T>();
}

template<typename T>
void to_json(rapidjson::Document& document, rapidjson::Value& json, const T& value) {
    json.Set<T>(value);
}

template<>
inline void from_json<std::string>(const rapidjson::Value& json, std::string& value) {
    value = json.GetString();
}

template<>
inline void to_json<std::string>(rapidjson::Document& document, rapidjson::Value& json, const std::string& value) {
    json.SetString(value.c_str(), document.GetAllocator());
}

template<>
inline void from_json(const rapidjson::Value& json, std::filesystem::path& value) {
    value = json.GetString();
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const std::filesystem::path& value) {
    to_json(document, json, value.string());
}

namespace detail {

template<typename VecT>
static VecT parse_vec(const rapidjson::Value& member) {
    using T = typename VecT::value_type;
    std::stringstream str(member.GetString());
    std::string segment;
    VecT result = {};
    for (int i = 0; std::getline(str, segment, ';') && i < VecT::length(); i++) {
        if constexpr (std::is_same_v<T, float>) {
            result[i] = std::stof(segment);
        } else if constexpr (std::is_same_v<T, double>) {
            result[i] = std::stod(segment);
        } else if constexpr (std::is_same_v<T, int32_t>) {
            result[i] = std::stoi(segment);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            result[i] = std::stoul(segment);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result[i] = std::stoll(segment);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            result[i] = std::stoull(segment);
        } else if constexpr (std::is_same_v<T, bool>) {
            result[i] = std::stoi(segment) != 0;
        } else {
            static_assert(false, "Unsupported type");
        }
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

}

#endif //DUK_SERIAL_JSON_CONVERSION_H