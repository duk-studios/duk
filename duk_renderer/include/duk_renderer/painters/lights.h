/// 13/10/2023
/// lights.h

#ifndef DUK_RENDERER_LIGHTS_H
#define DUK_RENDERER_LIGHTS_H

#include <duk_renderer/painters/uniform_buffer.h>

#include <glm/matrix.hpp>

namespace duk::renderer {

struct LightValue {
    glm::vec3 color;
    float intensity;
};

struct DirectionalLight {
    LightValue value;
    glm::vec3 direction;
};

struct Lights {
    DirectionalLight directionalLight;
};

using LightsUBO = UniformBuffer<Lights>;

}
#endif // DUK_RENDERER_LIGHTS_H

