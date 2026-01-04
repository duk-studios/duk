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

namespace duk::type {

template<>
struct Type<rhi::Sampler::Filter> : Enum<rhi::Sampler::Filter,
    Value<"nearest", rhi::Sampler::Filter::NEAREST>,
    Value<"linear", rhi::Sampler::Filter::LINEAR>,
    Value<"cubic", rhi::Sampler::Filter::CUBIC>> {
};

template<>
struct Type<rhi::Sampler::WrapMode> : Enum<rhi::Sampler::WrapMode,
    Value<"repeat", rhi::Sampler::WrapMode::REPEAT>,
    Value<"mirror-repeat", rhi::Sampler::WrapMode::MIRRORED_REPEAT>,
    Value<"clamp-edge", rhi::Sampler::WrapMode::CLAMP_TO_EDGE>,
    Value<"clamp-border", rhi::Sampler::WrapMode::CLAMP_TO_BORDER>,
    Value<"mirror-clamp-edge", rhi::Sampler::WrapMode::MIRROR_CLAMP_TO_EDGE>> {
};

template<>
struct Type<rhi::Sampler> : Class<rhi::Sampler,
    Member<"filter", &rhi::Sampler::filter>,
    Member<"wrap", &rhi::Sampler::wrapMode>> {
};

}

#endif// DUK_RHI_IMAGE_SAMPLER_H
