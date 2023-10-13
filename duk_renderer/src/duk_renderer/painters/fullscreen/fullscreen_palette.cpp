/// 05/10/2023
/// fullscreen_palette.cpp

#include <duk_renderer/painters/fullscreen/fullscreen_palette.h>
#include <duk_renderer/painters/fullscreen/fullscreen_painter.h>

namespace duk::renderer {


FullscreenPalette::FullscreenPalette(const FullscreenPaletteCreateInfo& fullscreenPaletteCreateInfo) {
    auto rhi = fullscreenPaletteCreateInfo.rhi;
    auto painter = fullscreenPaletteCreateInfo.painter;

    duk::rhi::RHI::DescriptorSetCreateInfo globalDescriptorSet = {};
    globalDescriptorSet.description = painter->descriptor_set_description();

    auto expectedGlobalDescriptorSet = rhi->create_descriptor_set(globalDescriptorSet);

    if (!expectedGlobalDescriptorSet) {
        throw std::runtime_error("failed to create ColorPalette global descriptor set: " + expectedGlobalDescriptorSet.error().description());
    }

    m_descriptorSet = std::move(expectedGlobalDescriptorSet.value());
}

void FullscreenPalette::apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) {
    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

void FullscreenPalette::update(duk::rhi::Image* image, const duk::rhi::Sampler& sampler) {
    m_descriptorSet->set(0, duk::rhi::Descriptor::image_sampler(image, duk::rhi::Image::Layout::SHADER_READ_ONLY, sampler));
    m_descriptorSet->flush();
}

}
