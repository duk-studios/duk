/// 16/05/2023
/// std_shader_data_source.h

#ifndef DUK_RHI_STD_SHADER_DATA_SOURCE_H
#define DUK_RHI_STD_SHADER_DATA_SOURCE_H

#include <duk_rhi/pipeline/shader_data_source.h>

namespace duk::rhi {

class StdShaderDataSource : public ShaderDataSource {
public:
    StdShaderDataSource();

    ~StdShaderDataSource() override;

    void insert_spir_v_code(Shader::Module::Bits type, const uint8_t* data, size_t size);

    void insert_spir_v_code(Shader::Module::Bits type, const std::vector<uint8_t>& data);

    void insert_spir_v_code(Shader::Module::Bits type, std::vector<uint8_t>&& data);

    void insert_descriptor_set_description(const DescriptorSetDescription& descriptorSetDescription);

    void insert_descriptor_set_description(DescriptorSetDescription&& descriptorSetDescription);

    void insert_vertex_attribute(VertexAttribute::Format format);

    void insert_vertex_attributes(const std::initializer_list<VertexAttribute::Format>& formats);

    // overrides
    Shader::Module::Mask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(Shader::Module::Bits type) const override;

    const std::vector<DescriptorSetDescription>& descriptor_set_descriptions() const override;

    const VertexLayout& vertex_layout() const override;

protected:
    hash::Hash calculate_hash() const override;

    struct ShaderModule {
        std::vector<uint8_t> spirVCode;
    };

    std::unordered_map<Shader::Module::Bits, ShaderModule> m_shaderModules;
    Shader::Module::Mask m_moduleMask;
    std::vector<DescriptorSetDescription> m_descriptorSetDescriptions;
    VertexLayout m_vertexLayout;

};
}

#endif // DUK_RHI_STD_SHADER_DATA_SOURCE_H

