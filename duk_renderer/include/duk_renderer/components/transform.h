/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_scene/objects.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

namespace duk::renderer {

struct Position2D {
    glm::vec2 value;
};

struct Position3D {
    glm::vec3 value;
};

struct Rotation2D {
    float value;
};

struct Rotation3D {
    glm::quat value;
};

struct Scale2D {
    glm::vec2 value{1};
};

struct Scale3D {
    glm::vec3 value{1};
};

glm::mat4 model_matrix_3d(const duk::scene::Object& object);

glm::mat3 model_matrix_2d(const duk::scene::Object& object);

glm::vec3 forward_direction_3d(const duk::scene::Object& object);

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Position3D& position) {
    visitor->visit_member(position.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Position2D& position) {
    visitor->visit_member(position.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Rotation3D& rotation) {
    visitor->visit_member(rotation.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Rotation2D& rotation) {
    visitor->visit_member(rotation.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Scale3D& scale) {
    visitor->visit_member(scale.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Scale2D& scale) {
    visitor->visit_member(scale.value, MemberDescription("value"));
}

}// namespace duk::serial

#endif// DUK_RENDERER_TRANSFORM_H
