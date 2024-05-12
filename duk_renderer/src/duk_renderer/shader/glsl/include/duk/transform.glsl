#ifndef DUK_TRANSFORM_GLSL
#define DUK_TRANSFORM_GLSL

#include "duk/instance.glsl"

struct Transform {
    mat4 model;
    mat4 invModel;
};

layout(set = 0, binding = 1) buffer TransformSBO {
    DUK_INSTANCE_DECLARE(Transform);
} uTransform;

mat4 duk_model(int instanceIndex) {
    return DUK_INSTANCE_GET(uTransform, instanceIndex).model;
}

mat4 duk_inverse_model(int instanceIndex) {
    return DUK_INSTANCE_GET(uTransform, instanceIndex).invModel;
}

vec4 duk_model_to_local(int instanceIndex, vec4 modelPosition) {
    return duk_model(instanceIndex) * modelPosition;
}

vec4 duk_local_to_model(int instanceIndex, vec4 localPosition) {
    return duk_inverse_model(instanceIndex) * localPosition;
}

#endif
