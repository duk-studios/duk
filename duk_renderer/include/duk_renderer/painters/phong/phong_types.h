#include <duk_renderer/painters/uniform_buffer.h>
#include <duk_renderer/painters/storage_buffer.h>
#include <glm/glm.hpp>

namespace duk::renderer::phong {

struct Material {
    glm::vec3 color;
    float shininess;
    float value;
    float foo;
    uint8_t _padding_foo[8];
};

struct Transform {
    glm::mat4x4 model;
    glm::mat4x4 invModel;
};

using TransformSBO = StorageBuffer<Transform>;
using MaterialUBO = UniformBuffer<Material>;

} // namespace duk::renderer::phong
