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

layout (set = 0, binding = 2) uniform LightsUBO {
    DirectionalLight directionalLights[8];
    int directionalLightCount;
    PointLight pointLights[32];
    int pointLightCount;
} uLights;

#endif // DUK_LIGHTING_GLSL
