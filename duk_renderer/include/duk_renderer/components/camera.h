/// 13/10/2023
/// camera_perspective.h

#ifndef DUK_RENDERER_CAMERA_H
#define DUK_RENDERER_CAMERA_H

#include <duk_scene/scene.h>

#include <glm/mat4x4.hpp>

namespace duk::renderer {

struct PerspectiveCamera {
    float fovDegrees;
    float aspectRatio;
    float zNear;
    float zFar;
};

glm::mat4 calculate_projection(const duk::scene::Object& object);

}

#endif // DUK_RENDERER_CAMERA_H

