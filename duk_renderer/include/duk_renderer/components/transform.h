/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_objects/objects.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

namespace duk::renderer {

struct Transform {
    glm::vec3 position{0};
    glm::quat rotation{glm::vec3(0)};
    glm::vec3 scale{1};

    // internal use
    glm::mat4 model{1};
    glm::mat4 invModel{1};
};

glm::vec3 forward(const Transform& transform);

void update_transform(const duk::objects::Component<Transform>& transform);

void update_transforms(duk::objects::Objects& objects);

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Transform& transform) {
    visitor->visit_member(transform.position, MemberDescription("position"));
    visitor->visit_member(transform.rotation, MemberDescription("rotation"));
    visitor->visit_member(transform.scale, MemberDescription("scale"));
}

}// namespace duk::serial

#endif// DUK_RENDERER_TRANSFORM_H
