/// 05/10/2023
/// fullscreen_palette.cpp

#include <duk_renderer/painters/fullscreen/fullscreen_palette.h>

namespace duk::renderer {


FullscreenPalette::FullscreenPalette(const FullscreenPaletteCreateInfo& fullscreenPaletteCreateInfo) :
    m_descriptorSet({fullscreenPaletteCreateInfo.renderer->rhi(), fullscreenPaletteCreateInfo.shaderDataSource}){

}

void FullscreenPalette::apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) {
    commandBuffer->bind_descriptor_set(m_descriptorSet.handle(), 0);
}

void FullscreenPalette::update(duk::rhi::Image* image, const duk::rhi::Sampler& sampler) {
    m_descriptorSet.set(FullscreenDescriptorSet::Bindings::uTexture, duk::rhi::Descriptor::image_sampler(image, duk::rhi::Image::Layout::SHADER_READ_ONLY, sampler));
    m_descriptorSet.flush();
}

}
