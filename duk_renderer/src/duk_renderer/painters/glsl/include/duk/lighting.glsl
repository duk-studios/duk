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

struct Lights {
    DirectionalLight directionalLights[16];
    int directionalLightCount;
};

#define DUK_DECLARE_LIGHTS_BINDING(bindingIndex, bindingName)   \
layout (binding = bindingIndex) uniform LightsUBO {             \
    Lights lights;                                              \
} bindingName

#endif // DUK_LIGHTING_GLSL
