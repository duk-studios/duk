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

glm::vec3 screen_to_local(const duk::scene::Component<Camera>& camera, const glm::vec2& screenSize, const glm::vec3& screenPosition);

glm::vec3 screen_to_world(const duk::scene::Component<Camera>& camera, const glm::vec2& screenSize, const glm::vec3& screenPosition);

void update_camera(const duk::scene::Component<Camera>& camera, const duk::scene::Component<PerspectiveCamera>& perspectiveCamera, uint32_t width, uint32_t height);

void update_cameras(duk::scene::Objects& objects, uint32_t width, uint32_t height);

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Camera& camera) {
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::PerspectiveCamera& camera) {
    visitor->visit_member(camera.fovDegrees, "fov");
    visitor->visit_member(camera.zNear, "near");
    visitor->visit_member(camera.zFar, "far");
}

}// namespace duk::serial

#endif// DUK_RENDERER_CAMERA_H
