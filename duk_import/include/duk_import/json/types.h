//
// Created by rov on 12/5/2023.
//

#ifndef DUK_IMPORT_TYPES_H
#define DUK_IMPORT_TYPES_H

#include <duk_pool/resource.h>
#include <duk_rhi/sampler.h>

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

}

#endif //DUK_IMPORT_TYPES_H
