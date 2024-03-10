/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_scene/objects.h>
#include <duk_scene/systems.h>

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

class TransformUpdateSystem : public duk::scene::System {
public:
    void enter(duk::scene::Objects& objects, duk::scene::Environment* environment) override;

    void update(duk::scene::Objects& objects, duk::scene::Environment* environment) override;

    void exit(duk::scene::Objects& objects, duk::scene::Environment* environment) override;
};

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
