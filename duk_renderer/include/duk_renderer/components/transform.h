/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_scene/objects.h>
#include <duk_json/types.h>

#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

namespace duk::renderer {

template<typename T>
struct Position {
    T value;
};

using Position2D = Position<glm::vec2>;
using Position3D = Position<glm::vec3>;

template<typename T>
struct Rotation {
    T value;
};

using Rotation2D = Rotation<float>;
using Rotation3D = Rotation<glm::quat>;


template<typename T>
struct Scale {
    T value {1};
};

using Scale2D = Scale<glm::vec2>;
using Scale3D = Scale<glm::vec3>;

glm::mat4 model_matrix_3d(const duk::scene::Object& object);

glm::mat3 model_matrix_2d(const duk::scene::Object& object);

glm::vec3 forward_direction_3d(const duk::scene::Object& object);

}

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
    object.value = glm::radians(from_json<glm::vec3>(jsonObject["value"])); // euler
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

}

#endif // DUK_RENDERER_TRANSFORM_H
