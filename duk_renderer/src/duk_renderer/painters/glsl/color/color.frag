#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "bindings.glsl"

layout(binding = 1, set = GLOBAL_SET_INDEX) uniform Material {
    vec4 color;
} uMaterial;

layout(location = 0) in vec4 vColor;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = vColor * uMaterial.color;
}