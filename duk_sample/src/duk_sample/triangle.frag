#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1, set = 0) uniform Material {
    vec4 color;
} uMaterial;

layout(binding = 2) uniform sampler2D uImage;


layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = vColor * uMaterial.color * texture(uImage, vTexCoord);
}