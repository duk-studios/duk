/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_objects/objects.h>

#include <duk_math/math.h>

namespace duk::renderer {

struct Transform {
    glm::vec3 position{0};
    glm::quat rotation{glm::vec3(0)};
    glm::vec3 scale{1};
};

struct Matrices {
    glm::mat4 model{1};
    glm::mat4 invModel{1};
    size_t hash{0};
};

glm::vec3 forward(const duk::objects::Component<Transform>& transform);

glm::vec3 world_position(const duk::objects::Component<Transform>& transform);

}// namespace duk::renderer

namespace duk::type {

template<>
struct Type<duk::renderer::Transform> : Class<duk::renderer::Transform,
    Member<"position", &duk::renderer::Transform::position>,
    Member<"rotation", &duk::renderer::Transform::rotation>,
    Member<"scale", &duk::renderer::Transform::scale>> {
};

}// namespace duk::type

#endif// DUK_RENDERER_TRANSFORM_H
