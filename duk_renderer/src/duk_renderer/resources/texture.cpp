/// 17/02/2024
/// texture.cpp

#include <duk_renderer/resources/texture.h>

namespace duk::renderer {

Texture::Texture() :
    Texture(duk::resource::Id(0), kDefaultTextureSampler) {

}

Texture::Texture(const ImageResource& image) :
    Texture(image, kDefaultTextureSampler) {

}

Texture::Texture(const ImageResource& image, const rhi::Sampler& sampler) :
    m_image(image),
    m_sampler(sampler) {

}

ImageResource& Texture::image() {
    return m_image;
}

const ImageResource& Texture::image() const {
    return m_image;
}

duk::rhi::Sampler& Texture::sampler() {
    return m_sampler;
}

const duk::rhi::Sampler& Texture::sampler() const {
    return m_sampler;
}

duk::rhi::Descriptor Texture::descriptor() {
    return duk::rhi::Descriptor::image_sampler(m_image->handle(), duk::rhi::Image::Layout::SHADER_READ_ONLY, m_sampler);
}

}