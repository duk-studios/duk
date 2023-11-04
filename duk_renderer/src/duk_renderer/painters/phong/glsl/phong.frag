#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

#include "duk/camera.glsl"
#include "duk/lighting.glsl"

DUK_DECLARE_CAMERA_BINDING(0, uCamera);
DUK_DECLARE_LIGHTS_BINDING(2, uLights);

struct Material {
    vec3 color;
    float shininess;
    float value;
    float foo;
};

layout(binding = 3) uniform MaterialUBO {
    Material material;
} uMaterial;

vec3 calculate_directional_lighting(in DirectionalLight light) {
    vec3 result = vec3(0);

    float intensity = clamp(dot(-vNormal, light.direction), 0.0, 1.0);

    result = light.value.color * light.value.intensity * intensity;

    return result;
}

void main() {
    vec3 lighting = calculate_directional_lighting(uLights.lights.directionalLights[0]);
    vec3 color = lighting * uMaterial.material.color;
    oColor = vec4(color, 1.0);
}