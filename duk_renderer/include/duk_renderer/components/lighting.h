/// 06/11/2023
/// lighting.h

#ifndef DUK_RENDERER_LIGHTING_H
#define DUK_RENDERER_LIGHTING_H

#include <duk_objects/objects.h>
#include <duk_math/math.h>

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

namespace duk::type {

// clang-format off
template<>
struct Type<duk::renderer::LightValue> : Class<duk::renderer::LightValue,
    Member<"color", &duk::renderer::LightValue::color>,
    Member<"intensity", &duk::renderer::LightValue::intensity>> {
};

template<>
struct Type<duk::renderer::DirectionalLight> : Class<duk::renderer::DirectionalLight,
    Member<"value", &duk::renderer::DirectionalLight::value>> {
};

template<>
struct Type<duk::renderer::PointLight> : Class<duk::renderer::PointLight,
    Member<"value", &duk::renderer::PointLight::value>,
    Member<"radius", &duk::renderer::PointLight::radius>> {
};

// clang-format on

}// namespace duk::type

#endif// DUK_RENDERER_LIGHTING_H
