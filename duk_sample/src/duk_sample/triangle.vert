#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0);
    vColor = aColor;
    vTexCoord = aTexCoord;
}