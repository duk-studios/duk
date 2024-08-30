#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/instance.glsl"

struct Properties {
    vec4 color;
};

layout(set = 1, binding = 1) buffer PropertiesSBO {
    DUK_INSTANCE_DECLARE(Properties);
} uProperties;

layout(set = 1, binding = 2) uniform sampler2D uImage;

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in flat int vInstanceIndex;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = DUK_INSTANCE_GET(uProperties, vInstanceIndex).color * texture(uImage, vTexCoord);
}