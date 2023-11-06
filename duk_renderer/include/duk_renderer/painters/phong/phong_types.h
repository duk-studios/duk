#ifndef DUK_RENDERER_PHONG_TYPES_H
#define DUK_RENDERER_PHONG_TYPES_H

#include <duk_renderer/painters/uniform_buffer.h>
#include <duk_renderer/painters/storage_buffer.h>
#include <glm/glm.hpp>

namespace duk::renderer::phong {

struct Transform {
    glm::mat4x4 model;
    glm::mat4x4 invModel;
};

using TransformSBO = StorageBuffer<Transform>;

} // namespace duk::renderer::phong

#endif // DUK_RENDERER_PHONG_TYPES_H
