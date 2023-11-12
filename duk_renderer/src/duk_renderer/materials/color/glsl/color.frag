#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Material {
    vec4 color;
};

layout(binding = 2) uniform MaterialUBO {
    Material value;
} uMaterial;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = uMaterial.value.color;
}