//
// Created by rov on 12/5/2023.
//

#include <duk_import/json/types.h>
#include <sstream>

namespace duk::import::json {

namespace detail {

template<typename VecT>
static VecT to_vec(const rapidjson::Value& member) {
    std::stringstream str(member.GetString());
    std::string segment;
    VecT result = {};
    for (int i = 0; std::getline(str, segment, ';') && i < VecT::length(); i++)
    {
        result[i] = std::stof(segment);
    }
    return result;
}

static duk::rhi::Sampler::WrapMode to_wrap_mode(const char* str) {
    if (strcmp(str, "repeat") == 0) {
        return rhi::Sampler::WrapMode::REPEAT;
    }
    if (strcmp(str, "mirror-repeat") == 0) {
        return rhi::Sampler::WrapMode::MIRRORED_REPEAT;
    }
    if (strcmp(str, "clamp-edge") == 0) {
        return rhi::Sampler::WrapMode::CLAMP_TO_EDGE;
    }
    if (strcmp(str, "clamp-border") == 0) {
        return rhi::Sampler::WrapMode::CLAMP_TO_BORDER;
    }
    if (strcmp(str, "mirror-clamp-edge") == 0) {
        return rhi::Sampler::WrapMode::MIRROR_CLAMP_TO_EDGE;
    }
    throw std::invalid_argument("unknown wrap mode name");
}

static duk::rhi::Sampler::Filter to_filter(const char* str) {
    if (strcmp(str, "nearest") == 0) {
        return rhi::Sampler::Filter::NEAREST;
    }
    if (strcmp(str, "linear") == 0) {
        return rhi::Sampler::Filter::LINEAR;
    }
    if (strcmp(str, "cubic") == 0) {
        return rhi::Sampler::Filter::CUBIC;
    }
    throw std::invalid_argument("unknown filter name");
}

}

glm::vec2 to_vec2(const rapidjson::Value& member) {
    return detail::to_vec<glm::vec2>(member);
}

glm::vec3 to_vec3(const rapidjson::Value& member) {
    return detail::to_vec<glm::vec3>(member);
}

glm::vec4 to_vec4(const rapidjson::Value& member) {
    return detail::to_vec<glm::vec4>(member);
}

duk::pool::ResourceId to_resource_id(const rapidjson::Value& member) {
    return duk::pool::ResourceId(member.GetUint64());
}

duk::rhi::Sampler::WrapMode to_wrap_mode(const rapidjson::Value& member) {
    return detail::to_wrap_mode(member.GetString());
}

duk::rhi::Sampler::Filter to_filter(const rapidjson::Value& member) {
    return detail::to_filter(member.GetString());
}

duk::rhi::Sampler to_sampler(const rapidjson::Value& member) {
    duk::rhi::Sampler sampler = {};
    sampler.wrapMode = to_wrap_mode(member["wrap"]);
    sampler.filter = to_filter(member["filter"]);
    return sampler;
}


}