/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_json/types.h>
#include <duk_scene/objects.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

namespace duk::renderer {

struct Position2D {
    glm::vec2 value;
};

struct Position3D {
    glm::vec3 value;
};

struct Rotation2D {
    float value;
};

struct Rotation3D {
    glm::quat value;
};

struct Scale2D {
    glm::vec2 value{1};
};

struct Scale3D {
    glm::vec3 value{1};
};

glm::mat4 model_matrix_3d(const duk::scene::Object& object);

glm::mat3 model_matrix_2d(const duk::scene::Object& object);

glm::vec3 forward_direction_3d(const duk::scene::Object& object);

}// namespace duk::renderer

namespace duk::json {

template<>
inline void from_json<duk::renderer::Position3D>(const rapidjson::Value& jsonObject, duk::renderer::Position3D& object) {
    object.value = from_json<glm::vec3>(jsonObject["value"]);
}

template<>
inline void from_json<duk::renderer::Position2D>(const rapidjson::Value& jsonObject, duk::renderer::Position2D& object) {
    object.value = from_json<glm::vec2>(jsonObject["value"]);
}

template<>
inline void from_json<duk::renderer::Rotation3D>(const rapidjson::Value& jsonObject, duk::renderer::Rotation3D& object) {
    object.value = glm::radians(from_json<glm::vec3>(jsonObject["value"]));// euler
}

template<>
inline void from_json<duk::renderer::Rotation2D>(const rapidjson::Value& jsonObject, duk::renderer::Rotation2D& object) {
    object.value = glm::radians(from_json<float>(jsonObject["value"]));
}

template<>
inline void from_json<duk::renderer::Scale3D>(const rapidjson::Value& jsonObject, duk::renderer::Scale3D& object) {
    object.value = from_json<glm::vec3>(jsonObject["value"]);
}

template<>
inline void from_json<duk::renderer::Scale2D>(const rapidjson::Value& jsonObject, duk::renderer::Scale2D& object) {
    object.value = from_json<glm::vec2>(jsonObject["value"]);
}

}// namespace duk::json

#endif// DUK_RENDERER_TRANSFORM_H
