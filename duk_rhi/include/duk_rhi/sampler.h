/// 23/04/2023
/// sampler.h

#ifndef DUK_RHI_IMAGE_SAMPLER_H
#define DUK_RHI_IMAGE_SAMPLER_H

#include <duk_macros/macros.h>
#include <duk_hash/hash.h>
#include <duk_json/types.h>

namespace duk::rhi {

struct Sampler {
public:

    enum class Filter {
        NEAREST,
        LINEAR,
        CUBIC
    };

    enum class WrapMode {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        MIRROR_CLAMP_TO_EDGE
    };

public:

    DUK_NO_DISCARD bool operator==(const Sampler& rhs) const noexcept;

public:
    Filter filter;
    WrapMode wrapMode;
};

}

namespace std {

template<>
struct hash<duk::rhi::Sampler> {
    size_t operator()(const duk::rhi::Sampler& sampler) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, sampler.filter);
        duk::hash::hash_combine(hash, sampler.wrapMode);
        return hash;
    }
};

}

namespace duk::json {

namespace detail {

static duk::rhi::Sampler::WrapMode parse_wrap_mode(const char* str) {
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

static duk::rhi::Sampler::Filter parse_filter(const char* str) {
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

template<>
inline void from_json<duk::rhi::Sampler::WrapMode>(const rapidjson::Value& jsonObject, duk::rhi::Sampler::WrapMode& object) {
    object = detail::parse_wrap_mode(jsonObject.GetString());
}

template<>
inline void from_json<duk::rhi::Sampler::Filter>(const rapidjson::Value& jsonObject, duk::rhi::Sampler::Filter& object) {
    object = detail::parse_filter(jsonObject.GetString());
}

template<>
inline void from_json<duk::rhi::Sampler>(const rapidjson::Value& jsonObject, duk::rhi::Sampler& object) {
    object.filter = from_json<duk::rhi::Sampler::Filter>(jsonObject["filter"]);
    object.wrapMode = from_json<duk::rhi::Sampler::WrapMode>(jsonObject["wrap"]);
}

}

#endif // DUK_RHI_IMAGE_SAMPLER_H

