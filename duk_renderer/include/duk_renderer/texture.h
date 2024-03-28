/// 17/02/2024
/// texture.h

#ifndef DUK_RENDERER_TEXTURE_H
#define DUK_RENDERER_TEXTURE_H

#include <duk_renderer/image/image.h>
#include <duk_resource/resource.h>
#include <duk_rhi/descriptor.h>
#include <duk_rhi/sampler.h>

namespace duk::renderer {

static constexpr duk::rhi::Sampler kDefaultTextureSampler(duk::rhi::Sampler::Filter::LINEAR, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE);

class ImagePool;

class Texture {
public:
    Texture();

    Texture(const ImageResource& image);

    Texture(const ImageResource& image, const duk::rhi::Sampler& sampler);

    DUK_NO_DISCARD ImageResource& image();

    DUK_NO_DISCARD const ImageResource& image() const;

    DUK_NO_DISCARD duk::rhi::Sampler& sampler();

    DUK_NO_DISCARD const duk::rhi::Sampler& sampler() const;

    DUK_NO_DISCARD duk::rhi::Descriptor descriptor();

private:
    ImageResource m_image;
    duk::rhi::Sampler m_sampler;
};

}// namespace duk::renderer

namespace std {

template<>
struct hash<duk::renderer::Texture> {
    size_t operator()(const duk::renderer::Texture& texture) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, texture.image().id());
        duk::hash::hash_combine(hash, texture.sampler());
        return hash;
    }
};

}// namespace std

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Texture& texture) {
    visitor->template visit_member<duk::resource::Resource>(texture.image(), MemberDescription("image"));
    visitor->visit_member_object(texture.sampler(), MemberDescription("sampler"));
}

}// namespace duk::serial

#endif// DUK_RENDERER_TEXTURE_H
