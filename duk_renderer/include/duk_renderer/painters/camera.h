/// 23/09/2023
/// camera.h

#ifndef DUK_RENDERER_CAMERA_H
#define DUK_RENDERER_CAMERA_H

#include <duk_renderer/painters/uniform_buffer.h>

#include <glm/matrix.hpp>

namespace duk::renderer {

struct CameraMatrices {
    glm::mat4 view;
    glm::mat4 invView;
    glm::mat4 proj;
    glm::mat4 vp;
};

using CameraUBO = UniformBuffer<CameraMatrices>;

}

#endif // DUK_RENDERER_CAMERA_H

