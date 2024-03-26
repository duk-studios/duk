/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_objects/objects.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

namespace duk::renderer {

struct Position3D {
    glm::vec3 value;
};

struct Rotation3D {
    glm::quat value;
};

struct Scale3D {
    glm::vec3 value{1};
};

struct Transform {
    glm::mat4 model;
    glm::mat4 invModel;
};

glm::vec3 forward(const Transform& transform);

void update_transform(const duk::scene::Component<Transform>& transform);

void update_transforms(duk::scene::Objects& objects);

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Position3D& position) {
    visitor->visit_member(position.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Rotation3D& rotation) {
    visitor->visit_member(rotation.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Scale3D& scale) {
    visitor->visit_member(scale.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Transform& transform) {
    // nothing for now
}

}// namespace duk::serial

#endif// DUK_RENDERER_TRANSFORM_H
