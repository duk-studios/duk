#ifndef DUK_LIGHTING_GLSL
#define DUK_LIGHTING_GLSL

struct LightValue {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    LightValue value;
    vec3 direction;
};

#define DUK_DECLARE_LIGHTS_BINDING(bindingIndex, bindingName)   \
layout (binding = bindingIndex) uniform LightsUBO {             \
    DirectionalLight directionalLights[1];                      \
} bindingName

#endif // DUK_LIGHTING_GLSL
