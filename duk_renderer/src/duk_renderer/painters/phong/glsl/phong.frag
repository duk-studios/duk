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

vec3 calculate_directional_lighting(in DirectionalLight light, in Material material, in vec3 normal, in vec3 view) {
    vec3 result = vec3(0);

    float diffuseIntensity = max(dot(normal, light.direction), 0.0);
    vec3 diffuse = (light.value.color * light.value.intensity) * (diffuseIntensity * material.diffuse);

    vec3 reflectDir = reflect(-light.direction, normal);
    float specularIntensity = pow(max(dot(view, reflectDir), 0.0), material.shininess);
    vec3 specular = (light.value.color * light.value.intensity) * (specularIntensity * material.specular);

    return diffuse + specular + material.ambient;
}

void main() {

    vec3 cameraPosition = vec3(uCamera.matrices.invView[3]);
    vec3 view = normalize(cameraPosition - vPosition);
    vec3 normal = normalize(vNormal);
    vec3 lighting = vec3(0);

    for (int i = 0; i < uLights.lights.directionalLightCount; i++) {
        lighting += calculate_directional_lighting(uLights.lights.directionalLights[i], uMaterial.material, normal, view);
    }

    oColor = vec4(lighting, 1.0);
}
