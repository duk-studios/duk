/// 13/10/2023
/// camera_perspective.h

#ifndef DUK_RENDERER_CAMERA_H
#define DUK_RENDERER_CAMERA_H

#include <duk_scene/objects.h>

#include <glm/mat4x4.hpp>

namespace duk::renderer {

struct Camera {};

struct PerspectiveCamera {
    float fovDegrees;
    float zNear;
    float zFar;
};

glm::mat4 calculate_projection(const duk::scene::Object& object, uint32_t width, uint32_t height);

glm::mat4 calculate_view(const duk::scene::Object& object);

}// namespace duk::renderer

namespace duk::json {

template<>
inline void from_json<duk::renderer::Camera>(const rapidjson::Value& jsonObject, duk::renderer::Camera& camera) {
}

template<>
inline void from_json<duk::renderer::PerspectiveCamera>(const rapidjson::Value& jsonObject, duk::renderer::PerspectiveCamera& perspectiveCamera) {
    from_json_member(jsonObject, "fov", perspectiveCamera.fovDegrees);
    from_json_member(jsonObject, "near", perspectiveCamera.zNear);
    from_json_member(jsonObject, "far", perspectiveCamera.zFar);
}

}// namespace duk::json

#endif// DUK_RENDERER_CAMERA_H
