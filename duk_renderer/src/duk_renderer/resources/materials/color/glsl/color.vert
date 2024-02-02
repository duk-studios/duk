#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/camera.glsl"
#include "duk/vertex.glsl"

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;
layout(location = 3) out vec4 vColor;


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
    vNormal = aNormal;
    vColor = aColor;
}