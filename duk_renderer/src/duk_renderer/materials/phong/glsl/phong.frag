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
};

layout(binding = 3) uniform MaterialUBO {
    Material material;
} uMaterial;

layout(binding = 4) uniform sampler2D uBaseColor;
layout(binding = 5) uniform sampler2D uSpecular;

vec3 calculate_lighting(in LightValue light, in float shininess, in vec3 normal, in vec3 view, in vec3 lightDirection, in float attenuation) {
    vec3 result = vec3(0);

    vec3 lighting = light.color * light.intensity * attenuation;

    float diffuseIntensity = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = lighting * diffuseIntensity;

    vec3 reflectDir = reflect(-lightDirection, normal);
    float specularIntensity = pow(max(dot(view, reflectDir), 0.0), shininess);
    vec3 specular = lighting * specularIntensity;

    return diffuse + specular;
}

vec3 calculate_directional_lighting(in DirectionalLight light, in float shininess, in vec3 normal, in vec3 view) {
    return calculate_lighting(light.value, shininess, normal, view, light.direction, 1.0f);
}

vec3 calculate_point_lighting(in PointLight light, in float shininess, in vec3 normal, in vec3 view, in vec3 position) {
    vec3 lightDirection = light.position - position;
    float distance = length(lightDirection);
    float attenuation = 1.0f / (1.0f + (light.linear * distance) + (light.quadratic * (distance * distance)));
    lightDirection = lightDirection / distance;

    return calculate_lighting(light.value, shininess, normal, view, lightDirection, attenuation);
}

void main() {
    vec3 cameraPosition = vec3(uCamera.matrices.invView[3]);
    vec3 view = normalize(cameraPosition - vPosition);
    vec3 normal = normalize(vNormal);
    vec3 lighting = vec3(0);

    float shininess = max(1, (uMaterial.material.shininess * texture(uSpecular, vTexCoord).r));

    for (int i = 0; i < uLights.lights.directionalLightCount; i++) {
        lighting += calculate_directional_lighting(uLights.lights.directionalLights[i], shininess, normal, view);
    }

    for (int i = 0; i < uLights.lights.pointLightCount; i++) {
        lighting += calculate_point_lighting(uLights.lights.pointLights[i], shininess, normal, view, vPosition);
    }

    vec4 color = texture(uBaseColor, vTexCoord) * vec4(uMaterial.material.color, 1.0f);

    oColor = color * vec4(lighting, 1.0);
}
