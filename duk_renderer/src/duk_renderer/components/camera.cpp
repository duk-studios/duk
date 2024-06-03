/// 13/10/2023
/// camera_perspective.cpp

#include <duk_objects/objects.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/transform.h>
#include <glm/ext/matrix_clip_space.hpp>

namespace duk::renderer {

namespace detail {

static float z_depth(const glm::mat4& projection, float z) {
    auto temp = (projection * glm::vec4(0, 0, z, 1));
    return (temp.z / temp.w + 1.0f) * 0.5f;
}

static glm::mat4 calculate_projection(const PerspectiveCamera& perspective, uint32_t width, uint32_t height) {
    const auto aspect = static_cast<float>(width) / static_cast<float>(height);
    return glm::perspective(glm::radians(perspective.fovDegrees), aspect, perspective.zNear, perspective.zFar);
}

}// namespace detail

glm::vec3 screen_to_local(const duk::objects::Component<Camera>& camera, const glm::vec2& screenSize, const glm::vec3& screenPosition) {
    auto ndc = glm::vec3(screenPosition.x / screenSize.x, 1.0 - screenPosition.y / screenSize.y, detail::z_depth(camera->proj, screenPosition.z)) * 2.0f - 1.0f;

    auto localPos = camera->invProj * glm::vec4(ndc, 1.0f);
    localPos /= localPos.w;

    return localPos;
}

glm::vec3 screen_to_world(const duk::objects::Component<Camera>& camera, const glm::vec2& screenSize, const glm::vec3& screenPosition) {
    auto local = screen_to_local(camera, screenSize, screenPosition);
    auto worldPos = camera->invView * glm::vec4(local, 1);

    return worldPos;
}

void update_camera(const duk::objects::Component<Camera>& camera, const duk::objects::Component<PerspectiveCamera>& perspectiveCamera, uint32_t width, uint32_t height) {
    auto transform = camera.object().component<Transform>();
    camera->view = transform->invModel;// the view is the inverse of the model
    camera->invView = transform->model;
    camera->proj = detail::calculate_projection(*perspectiveCamera, width, height);
    camera->invProj = glm::inverse(camera->proj);
    camera->vp = camera->proj * camera->view;
    camera->invVp = glm::inverse(camera->vp);
}

void update_cameras(duk::objects::Objects& objects, uint32_t width, uint32_t height) {
    for (auto object: objects.all_with<Camera, PerspectiveCamera>()) {
        auto [camera, perspective] = object.components<Camera, PerspectiveCamera>();
        update_camera(camera, perspective, width, height);
    }
}

}// namespace duk::renderer