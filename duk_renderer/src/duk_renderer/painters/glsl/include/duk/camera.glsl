#ifndef DUK_CAMERA_GLSL
#define DUK_CAMERA_GLSL

struct CameraMatrices {
    mat4 view;
    mat4 proj;
    mat4 vp;
};

#define DUK_DECLARE_CAMERA_BINDING(bindingIndex, bindingName)               \
layout (binding = bindingIndex) uniform CameraUBO {                         \
    CameraMatrices matrices;                                                \
} bindingName

#endif
