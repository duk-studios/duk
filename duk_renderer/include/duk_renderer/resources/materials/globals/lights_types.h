#ifndef DUK_RENDERER_LIGHTS_TYPES_H
#define DUK_RENDERER_LIGHTS_TYPES_H

#include <duk_renderer/resources/materials/storage_buffer.h>
#include <duk_renderer/resources/materials/uniform_buffer.h>
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

struct PointLight {
    LightValue value;
    glm::vec3 position;
    float linear;
    float quadratic;
    uint8_t _padding_quadratic[12];
};

struct Lights {
    DirectionalLight directionalLights[8];
    int directionalLightCount;
    uint8_t _padding_directionalLightCount[12];
    PointLight pointLights[32];
    int pointLightCount;
    uint8_t _padding_pointLightCount[12];
};

using LightsUBO = UniformBuffer<Lights>;

}// namespace duk::renderer::globals

#endif// DUK_RENDERER_LIGHTS_TYPES_H
