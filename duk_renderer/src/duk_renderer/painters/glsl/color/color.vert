#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 vColor;

layout(binding = 0, set = 0) uniform Transform {
    mat4 model;
} uTransform;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = uTransform.model * vec4(aPosition, 1.0);
    vColor = aColor;
}