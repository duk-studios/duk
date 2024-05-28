#ifndef DUK_CAMERA_GLSL
#define DUK_CAMERA_GLSL

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 vp;
} uCamera;

vec4 duk_local_to_world(vec4 localPosition) {
    return uCamera.view * localPosition;
}

vec4 duk_world_to_clip(vec4 worldPosition) {
    return uCamera.proj * worldPosition;
}

vec4 duk_local_to_clip(vec4 localPosition) {
    return uCamera.vp * localPosition;
}

vec4 duk_world_to_local(vec4 worldPosition) {
    return uCamera.invView * worldPosition;
}

vec4 duk_clip_to_local(vec4 clipPosition) {
    return inverse(uCamera.vp) * clipPosition;
}

vec4 duk_clip_to_world(vec4 clipPosition) {
    return inverse(uCamera.proj) * clipPosition;
}

vec3 duk_camera_position() {
    return uCamera.invView[3].xyz;
}

#endif // DUK_CAMERA_GLSL
