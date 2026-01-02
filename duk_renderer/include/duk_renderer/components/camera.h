/// 13/10/2023
/// camera_perspective.h

#ifndef DUK_RENDERER_CAMERA_H
#define DUK_RENDERER_CAMERA_H

#include <duk_objects/objects.h>
#include <duk_math/math.h>

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

namespace duk::type {

template<>
struct Type<duk::renderer::PerspectiveCamera> : Class<duk::renderer::PerspectiveCamera,
    Member<"fov", &duk::renderer::PerspectiveCamera::fovDegrees>,
    Member<"near", &duk::renderer::PerspectiveCamera::zNear>,
    Member<"far", &duk::renderer::PerspectiveCamera::zFar>> {
};


}// namespace duk::serial

#endif// DUK_RENDERER_CAMERA_H
