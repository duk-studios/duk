#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform MaterialUBO {
    vec4 color;
} uMaterial;

layout(binding = 3) uniform sampler2D uBaseColor;

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = uMaterial.color * texture(uBaseColor, vTexCoord).rrrr;
}