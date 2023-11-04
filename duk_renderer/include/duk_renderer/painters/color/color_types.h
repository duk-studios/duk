#include <duk_renderer/painters/uniform_buffer.h>
#include <duk_renderer/painters/storage_buffer.h>
#include <glm/glm.hpp>

namespace duk::renderer::color {

struct Instance {
    glm::mat4x4 model;
};

struct Material {
    glm::vec4 color;
};

using InstanceSBO = StorageBuffer<Instance>;
using MaterialUBO = UniformBuffer<Material>;

} // namespace duk::renderer::color
