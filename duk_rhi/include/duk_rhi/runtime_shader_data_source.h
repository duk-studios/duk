/// 16/05/2023
/// runtime_shader_data_source.h

#ifndef DUK_RHI_STD_SHADER_DATA_SOURCE_H
#define DUK_RHI_STD_SHADER_DATA_SOURCE_H

#include <duk_rhi/shader_data_source.h>
#include <duk_rhi/binding_layout.h>
#include <duk_rhi/vertex_layout.h>

#include <string_view>
#include <vector>

namespace duk::rhi {

/// All SPIR-V bytecodes that make up one shader program.
/// Leave a field empty (default) to omit that stage.
struct RuntimeShaderDataSourceCreateInfo {
    std::vector<uint8_t> vertexShaderCode;
    std::vector<uint8_t> tessellationControlShaderCode;
    std::vector<uint8_t> tessellationEvaluationShaderCode;
    std::vector<uint8_t> geometryShaderCode;
    std::vector<uint8_t> fragmentShaderCode;
    std::vector<uint8_t> computeShaderCode;
};

/// A runtime shader data source built from raw SPIR-V bytecodes.
/// The binding layout and vertex layout are derived by reflecting the SPIR-V
/// during construction. The object is immutable after construction.
class RuntimeShaderDataSource : public ShaderDataSource {
public:
    explicit RuntimeShaderDataSource(const RuntimeShaderDataSourceCreateInfo& createInfo);

    ~RuntimeShaderDataSource() override;

    // ShaderDataSource overrides
    ShaderModule::Mask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(ShaderModule::Bits type) const override;

    const std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>>& shader_modules() const override;

    const BindingLayout& binding_layout() const override;

    const VertexLayout& vertex_layout() const override;

    /// Returns the logical binding slot index for the given descriptor name.
    /// Throws std::out_of_range if the name is not present in the binding layout.
    DUK_NO_DISCARD uint32_t binding_index(std::string_view name) const;

protected:
    hash::Hash calculate_hash() const override;

private:
    std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>> m_shaderModules;
    ShaderModule::Mask m_moduleMask;
    BindingLayout m_bindingLayout;
    VertexLayout m_vertexLayout;
};

}// namespace duk::rhi

#endif// DUK_RHI_STD_SHADER_DATA_SOURCE_H
