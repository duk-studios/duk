/// 05/10/2023
/// fullscreen_material.cpp

#include <duk_renderer/materials/fullscreen/fullscreen_material.h>

namespace duk::renderer {

static const FullscreenShaderDataSource kFullscreenShaderDataSource;

FullscreenMaterial::FullscreenMaterial(const FullscreenMaterialCreateInfo& fullscreenMaterialCreateInfo) :
    Material(fullscreenMaterialCreateInfo.renderer, &kFullscreenShaderDataSource),
    m_descriptorSet({fullscreenMaterialCreateInfo.renderer->rhi(), &kFullscreenShaderDataSource}) {

}

void FullscreenMaterial::apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) {
    commandBuffer->bind_descriptor_set(m_descriptorSet.handle(), 0);
}

void FullscreenMaterial::update(duk::rhi::Image* image, const duk::rhi::Sampler& sampler) {
    m_descriptorSet.set(FullscreenDescriptorSet::Bindings::uTexture, duk::rhi::Descriptor::image_sampler(image, duk::rhi::Image::Layout::SHADER_READ_ONLY, sampler));
    m_descriptorSet.flush();
}


}
