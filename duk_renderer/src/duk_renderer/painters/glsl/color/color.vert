#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "camera.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 vColor;

layout(binding = 0, set = INSTANCE_SET_INDEX) buffer Transform {
    mat4 model[];
} uTransform;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = uCamera.matrices.vp * uTransform.model[gl_InstanceIndex] * vec4(aPosition, 1.0);
    vColor = aColor;
}