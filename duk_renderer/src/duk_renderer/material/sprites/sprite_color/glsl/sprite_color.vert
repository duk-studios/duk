#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/camera.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 vTexCoord;

DUK_DECLARE_CAMERA_BINDING(0, uCamera);

struct Instance {
    mat4 model;
};

layout(binding = 1) buffer InstanceSBO {
    Instance instances[];
} uInstances;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = uCamera.matrices.vp * uInstances.instances[gl_InstanceIndex].model * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
}