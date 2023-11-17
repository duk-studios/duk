#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

#include "duk/camera.glsl"
#include "duk/phong_lighting.glsl"

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

void main() {
    vec3 cameraPosition = vec3(uCamera.matrices.invView[3]);
    vec3 view = normalize(cameraPosition - vPosition);
    vec3 normal = normalize(vNormal);

    float shininess = max(1, (uMaterial.material.shininess * texture(uSpecular, vTexCoord).r));

    vec3 lighting = duk_calculate_phong_lights(uLights.lights, shininess, vPosition, normal, view);

    vec4 color = texture(uBaseColor, vTexCoord) * vec4(uMaterial.material.color, 1.0f);

    oColor = color * vec4(lighting, 1.0);
}
