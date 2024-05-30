/// 23/04/2023
/// sampler.h

#ifndef DUK_RHI_IMAGE_SAMPLER_H
#define DUK_RHI_IMAGE_SAMPLER_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>
#include <duk_serial/json/types.h>

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

}// namespace duk::rhi

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

}// namespace std

namespace duk::serial {

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

static std::string to_string(const duk::rhi::Sampler::WrapMode& wrapMode) {
    switch (wrapMode) {
        case rhi::Sampler::WrapMode::REPEAT:
            return "repeat";
        case rhi::Sampler::WrapMode::MIRRORED_REPEAT:
            return "mirror-repeat";
        case rhi::Sampler::WrapMode::CLAMP_TO_EDGE:
            return "clamp-edge";
        case rhi::Sampler::WrapMode::CLAMP_TO_BORDER:
            return "clamp-border";
        case rhi::Sampler::WrapMode::MIRROR_CLAMP_TO_EDGE:
            return "mirror-clamp-edge";
    }
    throw std::invalid_argument("unknown duk::rhi::Sampler::WrapMode");
}

static std::string to_string(const duk::rhi::Sampler::Filter& filter) {
    switch (filter) {
        case rhi::Sampler::Filter::NEAREST:
            return "nearest";
        case rhi::Sampler::Filter::LINEAR:
            return "linear";
        case rhi::Sampler::Filter::CUBIC:
            return "cubic";
    }
    throw std::invalid_argument("unknown duk::rhi::Sampler::Filter");
}

}// namespace detail

template<>
inline void from_json<duk::rhi::Sampler::WrapMode>(const rapidjson::Value& jsonObject, duk::rhi::Sampler::WrapMode& wrapMode) {
    wrapMode = detail::parse_wrap_mode(jsonObject.GetString());
}

template<>
inline void to_json<duk::rhi::Sampler::WrapMode>(rapidjson::Document& document, rapidjson::Value& json, const duk::rhi::Sampler::WrapMode& wrapMode) {
    auto wrapModeStr = detail::to_string(wrapMode);
    json.SetString(wrapModeStr.c_str(), wrapModeStr.size(), document.GetAllocator());
}

template<>
inline void from_json<duk::rhi::Sampler::Filter>(const rapidjson::Value& jsonObject, duk::rhi::Sampler::Filter& object) {
    object = detail::parse_filter(jsonObject.GetString());
}

template<>
inline void to_json<duk::rhi::Sampler::Filter>(rapidjson::Document& document, rapidjson::Value& json, const duk::rhi::Sampler::Filter& filter) {
    auto filterStr = detail::to_string(filter);
    json.SetString(filterStr.c_str(), filterStr.size(), document.GetAllocator());
}

template<>
inline void from_json<duk::rhi::Sampler>(const rapidjson::Value& json, duk::rhi::Sampler& sampler) {
    from_json_member(json, "filter", sampler.filter);
    from_json_member(json, "wrap", sampler.wrapMode);
}

template<>
inline void to_json<duk::rhi::Sampler>(rapidjson::Document& document, rapidjson::Value& json, const duk::rhi::Sampler& sampler) {
    to_json_member(document, json, "filter", sampler.filter);
    to_json_member(document, json, "wrap", sampler.wrapMode);
}

}// namespace duk::serial

#endif// DUK_RHI_IMAGE_SAMPLER_H
