//
// Created by rov on 12/5/2023.
//

#ifndef DUK_JSON_TYPES_H
#define DUK_JSON_TYPES_H

#include <rapidjson/document.h>
#include <glm/glm.hpp>
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

}

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

}

#endif //DUK_JSON_TYPES_H
