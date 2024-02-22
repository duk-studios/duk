/// 06/11/2023
/// lighting.h

#ifndef DUK_RENDERER_LIGHTING_H
#define DUK_RENDERER_LIGHTING_H

#include <duk_json/types.h>
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

namespace duk::json {

template<>
inline void from_json<duk::renderer::LightValue>(const rapidjson::Value& jsonObject, duk::renderer::LightValue& object) {
    object.color = from_json<glm::vec3>(jsonObject["color"]);
    object.intensity = from_json<float>(jsonObject["intensity"]);
}

template<>
inline void from_json<duk::renderer::DirectionalLight>(const rapidjson::Value& jsonObject, duk::renderer::DirectionalLight& object) {
    object.value = from_json<duk::renderer::LightValue>(jsonObject["value"]);
}

template<>
inline void from_json<duk::renderer::PointLight>(const rapidjson::Value& jsonObject, duk::renderer::PointLight& object) {
    object.value = from_json<duk::renderer::LightValue>(jsonObject["value"]);
    object.radius = from_json<float>(jsonObject["radius"]);
}

}// namespace duk::json

#endif// DUK_RENDERER_LIGHTING_H
