/// 13/10/2023
/// camera_perspective.h

#ifndef DUK_RENDERER_CAMERA_H
#define DUK_RENDERER_CAMERA_H

#include <duk_objects/objects.h>

#include <glm/mat4x4.hpp>

namespace duk::renderer {

struct Camera {
    glm::mat4 view;
    glm::mat4 invView;
    glm::mat4 proj;
    glm::mat4 invProj;
    glm::mat4 vp;   // projection * view
    glm::mat4 invVp;// inverse vp
};

struct PerspectiveCamera {
    float fovDegrees;
    float zNear;
    float zFar;
};

glm::vec3 screen_to_local(const duk::objects::Component<Camera>& camera, const glm::vec2& screenSize, const glm::vec3& screenPosition);

glm::vec3 screen_to_world(const duk::objects::Component<Camera>& camera, const glm::vec2& screenSize, const glm::vec3& screenPosition);

void update_camera(const duk::objects::Component<Camera>& camera, const duk::objects::Component<PerspectiveCamera>& perspectiveCamera, uint32_t width, uint32_t height);

void update_cameras(duk::objects::Objects& objects, uint32_t width, uint32_t height);

}// namespace duk::renderer

namespace duk::serial {

template<>
inline void from_json<duk::renderer::Camera>(const rapidjson::Value& json, duk::renderer::Camera& camera) {
}

template<>
inline void to_json<duk::renderer::Camera>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::Camera& camera) {
}

template<>
inline void from_json<duk::renderer::PerspectiveCamera>(const rapidjson::Value& json, duk::renderer::PerspectiveCamera& camera) {
    from_json_member(json, "fov", camera.fovDegrees);
    from_json_member(json, "near", camera.zNear);
    from_json_member(json, "far", camera.zFar);
}

template<>
inline void to_json<duk::renderer::PerspectiveCamera>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::PerspectiveCamera& camera) {
    to_json_member(document, json, "fov", camera.fovDegrees);
    to_json_member(document, json, "near", camera.zNear);
    to_json_member(document, json, "far", camera.zFar);
}

}// namespace duk::serial

#endif// DUK_RENDERER_CAMERA_H
