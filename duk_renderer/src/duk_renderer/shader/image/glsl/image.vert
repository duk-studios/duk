#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/vertex.glsl"
#include "duk/instance.glsl"
#include "duk/transform.glsl"
#include "duk/camera.glsl"

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out flat int vInstanceIndex;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vInstanceIndex = gl_InstanceIndex;
    gl_Position = duk_world_to_clip(duk_model_to_world(vInstanceIndex, vec4(aPosition, 1.0)));
    vTexCoord = aTexCoord;
}