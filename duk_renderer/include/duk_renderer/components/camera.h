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
