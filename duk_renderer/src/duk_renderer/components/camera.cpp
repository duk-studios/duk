/// 13/10/2023
/// camera_perspective.cpp

#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/transform.h>
#include <duk_scene/objects.h>
#include <glm/ext/matrix_clip_space.hpp>

namespace duk::renderer {

glm::mat4 calculate_projection(const duk::scene::Object& object, uint32_t width, uint32_t height) {
    float aspectRatio = (float)(width) / (float)(height);
    float fovDegrees = 45.0f;
    float zNear = 0.1f;
    float zFar = 1000.f;
    if (auto perspective = object.component<PerspectiveCamera>()) {
        fovDegrees = perspective->fovDegrees;
        zNear = perspective->zNear;
        zFar = perspective->zFar;
    }
    return glm::perspective(glm::radians(fovDegrees), aspectRatio, zNear, zFar);
}

glm::mat4 calculate_view(const scene::Object& object) {
    glm::vec3 position;
    glm::mat4 rotation;
    if (auto pos = object.component<Position3D>()) {
        position = pos->value;
    }
    if (auto rot = object.component<Rotation3D>()) {
        rotation = glm::mat4_cast(rot->value);
    }

    glm::vec3 forward(0, 0, -1);
    forward = rotation * glm::vec4(forward, 1.0f);
    forward += position;

    glm::vec3 up(0, 1, 0);
    up = rotation * glm::vec4(up, 0.0f);

    return glm::lookAt(position, forward, up);
}

}// namespace duk::renderer