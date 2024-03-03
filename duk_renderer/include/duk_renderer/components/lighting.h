/// 06/11/2023
/// lighting.h

#ifndef DUK_RENDERER_LIGHTING_H
#define DUK_RENDERER_LIGHTING_H

#include <duk_scene/objects.h>
#include <glm/glm.hpp>

namespace duk::renderer {

struct LightValue {
    glm::vec3 color;
    float intensity;
};

struct DirectionalLight {
    LightValue value;
};

struct PointLight {
    LightValue value;
    float radius;
};

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::LightValue& light) {
    visitor->visit_member(light.color, MemberDescription("color"));
    visitor->visit_member(light.intensity, MemberDescription("intensity"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::DirectionalLight& light) {
    visitor->visit_member_object(light.value, MemberDescription("value"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::PointLight& light) {
    visitor->visit_member_object(light.value, MemberDescription("value"));
    visitor->visit_member(light.radius, MemberDescription("radius"));
}

}// namespace duk::serial

#endif// DUK_RENDERER_LIGHTING_H
