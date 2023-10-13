#ifndef DUK_CAMERA_GLSL
#define DUK_CAMERA_GLSL

#include "bindings.glsl"

struct CameraMatrices {
    mat4 view;
    mat4 proj;
    mat4 vp;
};

layout (binding = 0, set = GLOBAL_SET_INDEX) uniform CameraUBO {
    CameraMatrices matrices;
} uCamera;

#endif
