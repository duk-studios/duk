#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/camera.glsl"
#include "duk/vertex.glsl"

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec2 vTexCoord;
layout(location = 2) out vec4 vColor;

DUK_DECLARE_CAMERA_BINDING(0, uCamera);

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = uCamera.matrices.vp * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
    vColor = aColor;
}