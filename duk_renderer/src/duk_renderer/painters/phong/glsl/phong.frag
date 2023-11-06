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

vec3 calculate_directional_lighting(in DirectionalLight light) {
    vec3 result = vec3(0);

    float intensity = clamp(dot(-vNormal, light.direction), 0.0, 1.0);

    result = light.value.color * light.value.intensity * intensity;

    return result;
}

void main() {
    vec3 lighting = calculate_directional_lighting(uLights.lights.directionalLights[0]);
    oColor = vec4(lighting, 1.0);
}
