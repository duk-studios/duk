#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/camera.glsl"
#include "duk/transform.glsl"
#include "duk/vertex.glsl"

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;
layout(location = 3) out flat int vInstanceIndex;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vInstanceIndex = gl_InstanceIndex;

    vPosition = duk_model_to_local(vInstanceIndex, vec4(aPosition, 1.0)).xyz;

    vNormal = duk_model_to_local_rotation(vInstanceIndex, aNormal);

    vTexCoord = aTexCoord;

    gl_Position = duk_local_to_clip(vec4(vPosition, 1.0));
}