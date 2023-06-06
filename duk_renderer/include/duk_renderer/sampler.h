/// 23/04/2023
/// image_sampler.h

#ifndef DUK_RENDERER_IMAGE_SAMPLER_H
#define DUK_RENDERER_IMAGE_SAMPLER_H

#include <duk_macros/macros.h>
#include <duk_hash/hash.h>

namespace duk::renderer {

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

    Filter filter;
    WrapMode wrapMode;
};

}

namespace std {

template<>
struct hash<duk::renderer::Sampler> {
    size_t operator()(const duk::renderer::Sampler& sampler) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, sampler.filter);
        duk::hash::hash_combine(hash, sampler.wrapMode);
        return hash;
    }
};

}

#endif // DUK_RENDERER_IMAGE_SAMPLER_H

