#ifndef DUK_RENDERER_PHONG_SHADER_DATA_SOURCE_H
#define DUK_RENDERER_PHONG_SHADER_DATA_SOURCE_H

#include <duk_rhi/pipeline/shader_data_source.h>

namespace duk::renderer {

class PhongShaderDataSource : public duk::rhi::ShaderDataSource {
public:
    PhongShaderDataSource();

    duk::rhi::ShaderModule::Mask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(duk::rhi::ShaderModule::Bits type) const override;

private:
    duk::hash::Hash calculate_hash() const override;

private:
    duk::rhi::ShaderModule::Mask m_moduleMask;
    std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>> m_moduleSpirVCode;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_PHONG_SHADER_DATA_SOURCE_H
