/// 13/10/2023
/// camera_perspective.cpp

#include <duk_renderer/components/camera_types.h>
#include <duk_scene/scene.h>
#include <glm/ext/matrix_clip_space.hpp>

namespace duk::renderer {

static const glm::mat4 kDefaultProjection = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 1000.0f);

glm::mat4 calculate_projection(const duk::scene::Object& object) {
    if (auto perspective = object.component<PerspectiveCamera>()) {
        return glm::perspective(glm::radians(perspective->fovDegrees), perspective->aspectRatio, perspective->zNear, perspective->zFar);
    }
    return kDefaultProjection;
}

}