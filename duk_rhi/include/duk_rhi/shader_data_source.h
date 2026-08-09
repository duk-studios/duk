/// 15/05/2023
/// shader_data_source.h

#ifndef DUK_RHI_SHADER_DATA_SOURCE_H
#define DUK_RHI_SHADER_DATA_SOURCE_H

#include <duk_rhi/shader.h>

#include <duk_hash/data_source.h>

#include <unordered_map>
#include <vector>

namespace duk::rhi {

class ShaderDataSource : public duk::hash::DataSource {
public:
    ~ShaderDataSource() override;

    DUK_NO_DISCARD virtual ShaderModule::Mask module_mask() const = 0;

    DUK_NO_DISCARD virtual const std::vector<uint8_t>& shader_module_spir_v_code(ShaderModule::Bits type) const = 0;

    /// All shader stage bytecodes keyed by stage bit.
    /// Backends use this to avoid per-stage copies when remapping binding numbers.
    DUK_NO_DISCARD virtual const std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>>& shader_modules() const = 0;

    /// Flat ordered binding layout for this shader.
    /// Generated data sources back this with a compile-time enum;
    /// runtime data sources derive it by reflecting the SPIR-V.
    DUK_NO_DISCARD virtual const BindingLayout& binding_layout() const = 0;

    /// Vertex attribute layout for this shader (empty for compute).
    /// Runtime data sources derive this by reflecting the vertex SPIR-V.
    DUK_NO_DISCARD virtual const VertexLayout& vertex_layout() const = 0;

    DUK_NO_DISCARD bool has_module(ShaderModule::Bits module) const;

    DUK_NO_DISCARD bool has_vertex_module() const;

    DUK_NO_DISCARD bool has_geometry_module() const;

    DUK_NO_DISCARD bool has_tesselation_control_module() const;

    DUK_NO_DISCARD bool has_tesselation_evaluate_module() const;

    DUK_NO_DISCARD bool has_fragment_module() const;

    DUK_NO_DISCARD bool has_compute_module() const;

    DUK_NO_DISCARD bool valid_graphics_shader() const;

    DUK_NO_DISCARD bool valid_compute_shader() const;
};

}// namespace duk::rhi

#endif// DUK_RHI_SHADER_DATA_SOURCE_H
