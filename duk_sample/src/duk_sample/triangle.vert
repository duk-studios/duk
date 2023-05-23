#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 vColor;

const vec2[3] kVertices = {
    vec2(-0.5, 0.3), // Top vertex
    vec2(0.5, 0.3), // Bottom left vertex
    vec2(0.0, -0.5)   // Bottom right vertex
};

const vec3[3] kColors = {
    vec3(0.79, 0.12, 0.33), // Top vertex color (red)
    vec3(0.02, 0.32, 0.59), // Bottom left vertex color (blue)
    vec3(0.21, 0.66, 0.44)   // Bottom right vertex color (green)
};


out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = vec4(kVertices[gl_VertexIndex], 0.0, 1.0);
    vColor = kColors[gl_VertexIndex];
}