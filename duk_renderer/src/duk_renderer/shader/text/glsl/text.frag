#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform Properties {
    vec4 color;
} uProperties;

layout(binding = 3) uniform sampler2D uBaseColor;

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = uProperties.color * texture(uBaseColor, vTexCoord).rrrr;
}