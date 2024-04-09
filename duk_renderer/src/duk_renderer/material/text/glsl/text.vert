#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/camera.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 vTexCoord;

DUK_DECLARE_CAMERA_BINDING(0, uCamera);

struct Transform {
    mat4 model;
};

layout(binding = 1) uniform InstanceUBO {
    Transform transform;
} uInstance;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = uCamera.matrices.vp * uInstance.transform.model * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
}