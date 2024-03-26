/// 13/10/2023
/// camera_perspective.h

#ifndef DUK_RENDERER_CAMERA_H
#define DUK_RENDERER_CAMERA_H

#include <duk_objects/objects.h>
#include <duk_objects/systems.h>

#include <glm/mat4x4.hpp>

namespace duk::renderer {

struct Camera {
    glm::mat4 view;
    glm::mat4 invView;
    glm::mat4 proj;
    glm::mat4 invProj;
    glm::mat4 vp;   // projection * view
    glm::mat4 invVp;// inverse vp

    glm::vec3 screen_to_world(const glm::vec2& screenSize, const glm::vec3& screenPosition) const;
    glm::vec3 screen_to_local(const glm::vec2& screenSize, const glm::vec3& screenPosition) const;
};

struct PerspectiveCamera {
    float fovDegrees;
    float zNear;
    float zFar;
};

class CameraUpdateSystem : public duk::scene::System {
public:
    void enter(scene::Objects& objects, scene::Environment* environment) override;

    void update(scene::Objects& objects, scene::Environment* environment) override;

    void exit(scene::Objects& objects, scene::Environment* environment) override;
};

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
