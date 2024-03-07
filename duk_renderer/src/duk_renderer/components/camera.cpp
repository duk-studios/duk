/// 13/10/2023
/// camera_perspective.cpp

#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/environment.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/renderer.h>
#include <duk_scene/objects.h>
#include <glm/ext/matrix_clip_space.hpp>

namespace duk::renderer {

namespace detail {

static float z_depth(const glm::mat4& projection, float z) {
    auto temp = (projection * glm::vec4(0, 0, z, 1));
    return (temp.z / temp.w + 1.0f) * 0.5f;
}

static glm::mat4 calculate_projection(const PerspectiveCamera& perspective, uint32_t width, uint32_t height) {
    return glm::perspective(glm::radians(perspective.fovDegrees), static_cast<float>(width) / static_cast<float>(height), perspective.zNear, perspective.zFar);
}

static void update_camera(const duk::scene::Object& object, Camera& camera, PerspectiveCamera& perspective, uint32_t width, uint32_t height) {
    camera.proj = detail::calculate_projection(perspective, width, height);
    camera.invProj = glm::inverse(camera.proj);
    camera.invView = model_matrix_3d(object);// the model matrix is the inverse of the view
    camera.view = glm::inverse(camera.invView);
    camera.vp = camera.proj * camera.view;
    camera.invVp = glm::inverse(camera.vp);
}

static void update_cameras(duk::scene::Objects& objects, uint32_t width, uint32_t height) {
    for (auto object: objects.all_with<Camera, PerspectiveCamera>()) {
        auto [camera, perspective] = object.components<Camera, PerspectiveCamera>();
        update_camera(object, *camera, *perspective, width, height);
    }
}

}// namespace detail



void CameraUpdateSystem::enter(duk::scene::Objects& objects, duk::scene::Environment* environment) {
    CameraUpdateSystem::update(objects, environment);// update cameras on enter
}

void CameraUpdateSystem::update(duk::scene::Objects& objects, duk::scene::Environment* environment) {
    auto renderer = environment->as<Environment>()->renderer();
    detail::update_cameras(objects, renderer->render_width(), renderer->render_height());
}

void CameraUpdateSystem::exit(duk::scene::Objects& objects, duk::scene::Environment* environment) {
}
}// namespace duk::renderer