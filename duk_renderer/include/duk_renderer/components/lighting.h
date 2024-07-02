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

namespace duk::serial {

template<>
inline void from_json<duk::renderer::LightValue>(const rapidjson::Value& json, duk::renderer::LightValue& light) {
    from_json_member(json, "color", light.color);
    from_json_member(json, "intensity", light.intensity);
}

template<>
inline void to_json<duk::renderer::LightValue>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::LightValue& light) {
    to_json_member(document, json, "color", light.color);
    to_json_member(document, json, "intensity", light.intensity);
}

template<>
inline void from_json<duk::renderer::DirectionalLight>(const rapidjson::Value& json, duk::renderer::DirectionalLight& light) {
    from_json_member(json, "value", light.value);
}

template<>
inline void to_json<duk::renderer::DirectionalLight>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::DirectionalLight& light) {
    to_json_member(document, json, "value", light.value);
}

template<>
inline void from_json<duk::renderer::PointLight>(const rapidjson::Value& json, duk::renderer::PointLight& light) {
    from_json_member(json, "value", light.value);
    from_json_member(json, "radius", light.radius);
}

template<>
inline void to_json<duk::renderer::PointLight>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::PointLight& light) {
    to_json_member(document, json, "value", light.value);
    to_json_member(document, json, "radius", light.radius);
}

}// namespace duk::serial

#endif// DUK_RENDERER_LIGHTING_H
