#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D uTexture;

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform Properties {
    vec2 scale;
} uProperties;

void main() {
    // Center the texture by adjusting UV coordinates
    vec2 centeredCoord = (vTexCoord - 0.5) / uProperties.scale + 0.5;
    outColor = vec4(texture(uTexture, centeredCoord).rgb, 1);
}
