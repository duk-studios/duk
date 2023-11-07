#ifndef DUK_RENDERER_PHONG_TYPES_H
#define DUK_RENDERER_PHONG_TYPES_H

#include <duk_renderer/painters/uniform_buffer.h>
#include <duk_renderer/painters/storage_buffer.h>
#include <glm/glm.hpp>

namespace duk::renderer::phong {

struct Material {
    glm::vec3 ambient;
    uint8_t _padding_ambient[4];
    glm::vec3 diffuse;
    uint8_t _padding_diffuse[4];
    glm::vec3 specular;
    float shininess;
};

struct Transform {
    glm::mat4x4 model;
    glm::mat4x4 invModel;
};

using TransformSBO = StorageBuffer<Transform>;
using MaterialUBO = UniformBuffer<Material>;

} // namespace duk::renderer::phong

#endif // DUK_RENDERER_PHONG_TYPES_H
