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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

layout(binding = 3) uniform MaterialUBO {
    Material material;
} uMaterial;

vec3 calculate_lighting(in LightValue light, in Material material, in vec3 normal, in vec3 view, in vec3 lightDirection, in float attenuation) {
    vec3 result = vec3(0);

    vec3 lighting = light.color * light.intensity * attenuation;

    float diffuseIntensity = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = lighting * (diffuseIntensity * material.diffuse);

    vec3 reflectDir = reflect(-lightDirection, normal);
    float specularIntensity = pow(max(dot(view, reflectDir), 0.0), material.shininess);
    vec3 specular = lighting * (specularIntensity * material.specular);

    return diffuse + specular;
}

vec3 calculate_directional_lighting(in DirectionalLight light, in Material material, in vec3 normal, in vec3 view) {
    return calculate_lighting(light.value, material, normal, view, light.direction, 1.0f);
}

vec3 calculate_point_lighting(in PointLight light, in Material material, in vec3 normal, in vec3 view, in vec3 position) {
    vec3 lightDirection = light.position - position;
    float distance = length(lightDirection);
    float attenuation = 1.0f / (1.0f + (light.linear * distance) + (light.quadratic * (distance * distance)));
    lightDirection = lightDirection / distance;

    return calculate_lighting(light.value, material, normal, view, lightDirection, attenuation);
}

void main() {

    vec3 cameraPosition = vec3(uCamera.matrices.invView[3]);
    vec3 view = normalize(cameraPosition - vPosition);
    vec3 normal = normalize(vNormal);
    vec3 lighting = vec3(0);

    for (int i = 0; i < uLights.lights.directionalLightCount; i++) {
        lighting += calculate_directional_lighting(uLights.lights.directionalLights[i], uMaterial.material, normal, view);
    }

    for (int i = 0; i < uLights.lights.pointLightCount; i++) {
        lighting += calculate_point_lighting(uLights.lights.pointLights[i], uMaterial.material, normal, view, vPosition);
    }

    oColor = vec4(lighting, 1.0);
}
