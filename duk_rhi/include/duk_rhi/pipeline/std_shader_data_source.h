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

    void insert_spir_v_code(ShaderModule::Bits type, const uint8_t* data, size_t size);

    void insert_spir_v_code(ShaderModule::Bits type, const std::vector<uint8_t>& data);

    void insert_spir_v_code(ShaderModule::Bits type, std::vector<uint8_t>&& data);

    // overrides
    ShaderModule::Mask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(ShaderModule::Bits type) const override;

protected:
    hash::Hash calculate_hash() const override;

    struct ShaderModuleCode {
        std::vector<uint8_t> spirVCode;
    };

    std::unordered_map<ShaderModule::Bits, ShaderModuleCode> m_shaderModules;
    ShaderModule::Mask m_moduleMask;
};
}// namespace duk::rhi

#endif// DUK_RHI_STD_SHADER_DATA_SOURCE_H
