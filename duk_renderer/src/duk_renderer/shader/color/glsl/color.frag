#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/instance.glsl"

struct Properties {
    vec4 color;
};

layout(binding = 2) buffer PropertiesSBO {
    DUK_INSTANCE_DECLARE(Properties);
} uProperties;

layout(binding = 3) uniform sampler2D uBaseColor;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in flat int vInstanceIndex;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = texture(uBaseColor, vTexCoord) * DUK_INSTANCE_GET(uProperties, vInstanceIndex).color;
}