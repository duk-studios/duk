#ifndef DUK_RENDERER_CAMERA_TYPES_H
#define DUK_RENDERER_CAMERA_TYPES_H

#include <duk_renderer/material/uniform_buffer.h>

namespace duk::renderer::globals {

struct CameraMatrices {
    glm::mat4x4 view;
    glm::mat4x4 invView;
    glm::mat4x4 proj;
    glm::mat4x4 vp;
};

using CameraUBO = UniformBuffer<CameraMatrices>;

}// namespace duk::renderer::globals

#endif// DUK_RENDERER_CAMERA_TYPES_H
