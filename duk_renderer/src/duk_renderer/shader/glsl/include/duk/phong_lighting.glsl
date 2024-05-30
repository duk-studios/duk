#ifndef DUK_PHONG_LIGHTING_GLSL
#define DUK_PHONG_LIGHTING_GLSL

#include "duk/lighting.glsl"

vec3 duk_calculate_phong_light(in LightValue light, in float shininess, in vec3 normal, in vec3 view, in vec3 lightDirection, in float attenuation) {
    vec3 result = vec3(0);

    vec3 lighting = light.color * light.intensity * attenuation;

    float diffuseIntensity = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = lighting * diffuseIntensity;

    vec3 specular = vec3(0);
    if (shininess <= 0.001) {
        vec3 reflectDir = reflect(lightDirection, normal);
        float specularIntensity = pow(max(dot(view, reflectDir), 0.0), shininess);
        specular = lighting * specularIntensity;
    }

    return diffuse + specular;
}

vec3 duk_calculate_phong_directional_light(in DirectionalLight light, in float shininess, in vec3 normal, in vec3 view) {
    return duk_calculate_phong_light(light.value, shininess, normal, view, light.direction, 1.0f);
}

vec3 duk_calculate_phong_point_light(in PointLight light, in float shininess, in vec3 normal, in vec3 view, in vec3 position) {
    vec3 lightDirection = light.position - position;
    float distance = length(lightDirection);
    float attenuation = 1.0f / (1.0f + (light.linear * distance) + (light.quadratic * (distance * distance)));
    lightDirection = lightDirection / distance;

    return duk_calculate_phong_light(light.value, shininess, normal, view, lightDirection, attenuation);
}

vec3 duk_calculate_phong_lights(in float shininess, in vec3 position, in vec3 normal, in vec3 view) {
    vec3 lighting = vec3(0);
    for (int i = 0; i < uLights.directionalLightCount; i++) {
        lighting += duk_calculate_phong_directional_light(uLights.directionalLights[i], shininess, normal, view);
    }

    for (int i = 0; i < uLights.pointLightCount; i++) {
        lighting += duk_calculate_phong_point_light(uLights.pointLights[i], shininess, normal, view, position);
    }
    return lighting;
}

#endif //DUK_PHONG_LIGHTING_GLSL