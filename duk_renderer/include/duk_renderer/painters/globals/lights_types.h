#include <duk_renderer/painters/uniform_buffer.h>
#include <duk_renderer/painters/storage_buffer.h>
#include <glm/glm.hpp>

namespace duk::renderer::globals {

struct LightValue {
    glm::vec3 color;
    float intensity;
};

struct DirectionalLight {
    LightValue value;
    glm::vec3 direction;
    uint8_t _padding_direction[4];
};

struct Lights {
    DirectionalLight directionalLights[16];
};

using LightsUBO = UniformBuffer<Lights>;

} // namespace duk::renderer::globals
