/// 23/04/2023
/// sampler.h

#ifndef DUK_RHI_IMAGE_SAMPLER_H
#define DUK_RHI_IMAGE_SAMPLER_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>
#include <duk_serial/json.h>

namespace duk::rhi {

struct Sampler {

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

    Filter filter;
    WrapMode wrapMode;
};

bool operator==(const Sampler& lhs, const Sampler& rhs) noexcept;

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

template<>
struct JsonPrimitiveValue<duk::rhi::Sampler::WrapMode> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const duk::rhi::Sampler::WrapMode& value);

    static void read(const rapidjson::Value& json, duk::rhi::Sampler::WrapMode& value);
};

template<>
struct JsonPrimitiveValue<duk::rhi::Sampler::Filter> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const duk::rhi::Sampler::Filter& value);

    static void read(const rapidjson::Value& json, duk::rhi::Sampler::Filter& value);
};

}// namespace duk::serial

namespace duk::type {

template<>
struct Type<rhi::Sampler> : Class<rhi::Sampler,
    Member<"filter", &rhi::Sampler::filter>,
    Member<"wrap", &rhi::Sampler::wrapMode>> {
};

}

#endif// DUK_RHI_IMAGE_SAMPLER_H
