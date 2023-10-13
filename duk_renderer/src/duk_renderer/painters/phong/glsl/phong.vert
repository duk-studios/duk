#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/camera.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;

DUK_DECLARE_CAMERA_BINDING(0, uCamera);

layout(binding = 1) buffer Transform {
    mat4 model[];
} uTransform;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    mat4 model = uTransform.model[gl_InstanceIndex];

    vPosition = vec3(model * vec4(aPosition, 1.0)).xyz;
    vNormal = normalize(model * vec4(aNormal, 0.0)).xyz;
    vTexCoord = aTexCoord;

    gl_Position = uCamera.matrices.vp * vec4(vPosition, 1.0);
}