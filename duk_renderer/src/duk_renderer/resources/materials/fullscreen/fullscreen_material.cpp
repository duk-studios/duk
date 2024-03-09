/// 05/10/2023
/// fullscreen_material.cpp

#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/materials/fullscreen/fullscreen_material.h>

namespace duk::renderer {

namespace detail {

static const FullscreenShaderDataSource kFullscreenShaderDataSource;

}

const duk::rhi::ShaderDataSource* FullscreenMaterialDataSource::shader_data_source() const {
    return &detail::kFullscreenShaderDataSource;
}

std::unique_ptr<MaterialDescriptorSet> FullscreenMaterialDataSource::create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const {
    FullscreenMaterialDescriptorSetCreateInfo fullscreenMaterialDescriptorSetCreateInfo = {};
    fullscreenMaterialDescriptorSetCreateInfo.renderer = materialDescriptorSetCreateInfo.renderer;
    fullscreenMaterialDescriptorSetCreateInfo.fullscreenMaterialDataSource = this;
    return std::make_unique<FullscreenMaterialDescriptorSet>(fullscreenMaterialDescriptorSetCreateInfo);
}

hash::Hash FullscreenMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, sampler);
    return hash;
}

FullscreenMaterialDescriptorSet::FullscreenMaterialDescriptorSet(const FullscreenMaterialDescriptorSetCreateInfo& fullscreenMaterialDescriptorSetCreateInfo) {
    auto rhi = fullscreenMaterialDescriptorSetCreateInfo.renderer->rhi();
    auto materialDataSource = fullscreenMaterialDescriptorSetCreateInfo.fullscreenMaterialDataSource;
    auto shaderDataSource = materialDataSource->shader_data_source();

    duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = shaderDataSource->descriptor_set_descriptions().at(0);
    m_descriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);

    m_sampler = materialDataSource->sampler;
}

void FullscreenMaterialDescriptorSet::set_image(duk::rhi::Image* image) {
    m_descriptorSet->set(FullscreenDescriptors::uTexture, duk::rhi::Descriptor::image_sampler(image, rhi::Image::Layout::SHADER_READ_ONLY, m_sampler));
    m_descriptorSet->flush();
}

void FullscreenMaterialDescriptorSet::update(const DrawParams& params) {
    // nothing to update
}

void FullscreenMaterialDescriptorSet::bind(duk::rhi::CommandBuffer* commandBuffer) {
    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

uint32_t FullscreenMaterialDescriptorSet::size() const {
    return FullscreenDescriptors::kDescriptorCount;
}

}// namespace duk::renderer
