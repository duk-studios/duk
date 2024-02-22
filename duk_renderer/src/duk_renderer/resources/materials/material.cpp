/// 12/09/2023
/// material.cpp

#include <duk_renderer/resources/materials/material.h>

namespace duk::renderer {

Material::Material(const MaterialCreateInfo& materialCreateInfo) : m_pipeline({.renderer = materialCreateInfo.renderer, .shaderDataSource = materialCreateInfo.materialDataSource->shader_data_source()}) {
    MaterialDescriptorSetCreateInfo materialDescriptorSetCreateInfo = {};
    materialDescriptorSetCreateInfo.renderer = materialCreateInfo.renderer;
    m_descriptorSet = materialCreateInfo.materialDataSource->create_descriptor_set(materialDescriptorSetCreateInfo);
}

Material::~Material() = default;

Pipeline* Material::pipeline() {
    return &m_pipeline;
}

const Pipeline* Material::pipeline() const {
    return &m_pipeline;
}

MaterialDescriptorSet* Material::descriptor_set() {
    return m_descriptorSet.get();
}

InstanceBuffer* Material::instance_buffer() {
    return m_descriptorSet->instance_buffer();
}

void Material::bind(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& drawParams) {
    m_pipeline.use(commandBuffer, drawParams);
    m_descriptorSet->bind(commandBuffer, drawParams);
}

}// namespace duk::renderer
