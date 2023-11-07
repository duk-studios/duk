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

struct PointLight {
    LightValue value;
    vec3 position;
    float linear;
    float quadratic;
};

struct Lights {
    DirectionalLight directionalLights[8];
    int directionalLightCount;
    PointLight pointLights[32];
    int pointLightCount;
};

#define DUK_DECLARE_LIGHTS_BINDING(bindingIndex, bindingName)   \
layout (binding = bindingIndex) uniform LightsUBO {             \
    Lights lights;                                              \
} bindingName

#endif // DUK_LIGHTING_GLSL
