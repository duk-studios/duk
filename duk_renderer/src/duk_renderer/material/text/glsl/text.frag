#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Material {
    vec4 color;
};

layout(binding = 2) uniform MaterialUBO {
    Material value;
} uMaterial;

layout(binding = 3) uniform sampler2D uBaseColor;

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = uMaterial.value.color * texture(uBaseColor, vTexCoord).rrrr;
//    oColor = vec4(1);
}