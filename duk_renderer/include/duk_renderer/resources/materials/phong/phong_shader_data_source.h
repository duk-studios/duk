#ifndef DUK_RENDERER_PHONG_SHADER_DATA_SOURCE_H
#define DUK_RENDERER_PHONG_SHADER_DATA_SOURCE_H

#include <duk_rhi/pipeline/shader_data_source.h>

namespace duk::renderer {

class PhongShaderDataSource : public duk::rhi::ShaderDataSource {
public:
    PhongShaderDataSource();

    duk::rhi::Shader::Module::Mask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(duk::rhi::Shader::Module::Bits type) const override;

    const std::vector<duk::rhi::DescriptorSetDescription>& descriptor_set_descriptions() const override;

    const duk::rhi::VertexLayout& vertex_layout() const override;

private:
    duk::hash::Hash calculate_hash() const override;

private:
    duk::rhi::Shader::Module::Mask m_moduleMask;
    std::unordered_map<duk::rhi::Shader::Module::Bits, std::vector<uint8_t>> m_moduleSpirVCode;
    std::vector<duk::rhi::DescriptorSetDescription> m_descriptorSetDescriptions;
    duk::rhi::VertexLayout m_vertexLayout;
};

} // namespace duk::renderer

#endif // DUK_RENDERER_PHONG_SHADER_DATA_SOURCE_H
