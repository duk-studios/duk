#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/instance.glsl"
#include "duk/transform.glsl"
#include "duk/camera.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 vTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = duk_world_to_clip(duk_model_to_world(gl_InstanceIndex, vec4(aPosition, 1.0)));
    vTexCoord = aTexCoord;
}