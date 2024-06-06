#ifndef DUK_CAMERA_GLSL
#define DUK_CAMERA_GLSL

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 vp;
} uCamera;

vec4 duk_world_to_view(vec4 worldPosition) {
    return uCamera.view * worldPosition;
}

vec4 duk_view_to_clip(vec4 viewPosition) {
    return uCamera.proj * viewPosition;
}

vec4 duk_world_to_clip(vec4 worldPosition) {
    return uCamera.vp * worldPosition;
}

vec4 duk_view_to_world(vec4 viewPosition) {
    return uCamera.invView * viewPosition;
}

vec4 duk_clip_to_world(vec4 clipPosition) {
    return inverse(uCamera.vp) * clipPosition;
}

vec4 duk_clip_to_view(vec4 clipPosition) {
    return inverse(uCamera.proj) * clipPosition;
}

vec3 duk_camera_position() {
    return uCamera.invView[3].xyz;
}

float duk_camera_aspect_ratio() {
    return uCamera.proj[0][0] / uCamera.proj[1][1];
}

#endif // DUK_CAMERA_GLSL
