/// 17/02/2024
/// texture.cpp

#include <duk_renderer/resources/texture.h>
#include <duk_renderer/pools/image_pool.h>

namespace duk::renderer {

Texture::Texture() :
    m_image(duk::resource::Id(0)),
    m_sampler(duk::rhi::Sampler::Filter::LINEAR, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE) {

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