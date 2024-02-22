/// 05/10/2023
/// fullscreen_material.h

#ifndef DUK_RENDERER_FULLSCREEN_MATERIAL_H
#define DUK_RENDERER_FULLSCREEN_MATERIAL_H

#include <duk_renderer/resources/materials/fullscreen/fullscreen_descriptors.h>
#include <duk_renderer/resources/materials/fullscreen/fullscreen_shader_data_source.h>
#include <duk_renderer/resources/materials/material.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class FullscreenMaterialDataSource : public MaterialDataSource {
public:
    const duk::rhi::ShaderDataSource* shader_data_source() const override;

    std::unique_ptr<MaterialDescriptorSet> create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const override;

    duk::rhi::Sampler sampler;

protected:
    hash::Hash calculate_hash() const override;
};

struct FullscreenMaterialDescriptorSetCreateInfo {
    Renderer* renderer;
    const FullscreenMaterialDataSource* fullscreenMaterialDataSource;
};

class FullscreenMaterialDescriptorSet : public MaterialDescriptorSet {
public:
    explicit FullscreenMaterialDescriptorSet(const FullscreenMaterialDescriptorSetCreateInfo& fullscreenMaterialDescriptorSetCreateInfo);

    void set_image(duk::rhi::Image* image);

    void bind(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params) override;

    uint32_t size() const override;

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    duk::rhi::Sampler m_sampler;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_FULLSCREEN_MATERIAL_H
