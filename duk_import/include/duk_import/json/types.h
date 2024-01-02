//
// Created by rov on 12/5/2023.
//

#ifndef DUK_IMPORT_TYPES_H
#define DUK_IMPORT_TYPES_H

#include <duk_pool/resource.h>
#include <duk_rhi/sampler.h>
#include <duk_tools/singleton.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/mesh_drawing.h>
#include <duk_renderer/components/lighting.h>

#include <rapidjson/document.h>
#include <glm/glm.hpp>

namespace duk::import::json {

glm::vec2 to_vec2(const rapidjson::Value& member);

glm::vec3 to_vec3(const rapidjson::Value& member);

glm::vec4 to_vec4(const rapidjson::Value& member);

duk::pool::ResourceId to_resource_id(const rapidjson::Value& member);

duk::rhi::Sampler::WrapMode to_wrap_mode(const rapidjson::Value& member);

duk::rhi::Sampler::Filter to_filter(const rapidjson::Value& member);

duk::rhi::Sampler to_sampler(const rapidjson::Value& member);

duk::renderer::LightValue to_light_value(const rapidjson::Value& member);

template<typename T>
void from_json(const rapidjson::Value& jsonObject, T& object) {
    object = jsonObject.Get<T>();
}

template<>
void from_json<glm::vec2>(const rapidjson::Value& jsonObject, glm::vec2& object) {
    object = to_vec2(jsonObject);
}

template<>
void from_json<glm::vec3>(const rapidjson::Value& jsonObject, glm::vec3& object) {
    object = to_vec3(jsonObject);
}

template<>
void from_json<glm::vec4>(const rapidjson::Value& jsonObject, glm::vec4& object) {
    object = to_vec4(jsonObject);
}

template<>
void from_json<duk::pool::ResourceId>(const rapidjson::Value& jsonObject, duk::pool::ResourceId& object) {
    object = to_resource_id(jsonObject);
}

template<>
void from_json<duk::rhi::Sampler::WrapMode>(const rapidjson::Value& jsonObject, duk::rhi::Sampler::WrapMode& object) {
    object = to_wrap_mode(jsonObject);
}

template<>
void from_json<duk::rhi::Sampler::Filter>(const rapidjson::Value& jsonObject, duk::rhi::Sampler::Filter& object) {
    object = to_filter(jsonObject);
}

template<>
void from_json<duk::rhi::Sampler>(const rapidjson::Value& jsonObject, duk::rhi::Sampler& object) {
    object = to_sampler(jsonObject);
}

template<>
void from_json<duk::renderer::LightValue>(const rapidjson::Value& jsonObject, duk::renderer::LightValue& object) {
    object = to_light_value(jsonObject);
}

}

#endif //DUK_IMPORT_TYPES_H
