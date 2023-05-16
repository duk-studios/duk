/// 16/05/2023
/// std_shader_data_source.h

#ifndef DUK_RENDERER_STD_SHADER_DATA_SOURCE_H
#define DUK_RENDERER_STD_SHADER_DATA_SOURCE_H

#include <duk_renderer/pipeline/shader_data_source.h>

namespace duk::renderer {

class StdShaderDataSource : public ShaderDataSource {
public:
    ~StdShaderDataSource() override;

    void insert_spir_v_code(Shader::ModuleType::Bits type, const uint8_t* data, size_t size);

    void insert_spir_v_code(Shader::ModuleType::Bits type, const std::vector<uint8_t>& data);

    void insert_spir_v_code(Shader::ModuleType::Bits type, std::vector<uint8_t>&& data);

    // overrides
    Shader::ModuleMask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(Shader::ModuleType::Bits type) const override;

protected:
    hash::Hash calculate_hash() const override;

    struct ShaderModule {
        std::vector<uint8_t> spirVCode;
    };

    std::unordered_map<Shader::ModuleType::Bits, ShaderModule> m_shaderModules;
    Shader::ModuleMask m_moduleMask;

};
}

#endif // DUK_RENDERER_STD_SHADER_DATA_SOURCE_H

