/// 15/05/2023
/// shader_data_source.h

#ifndef DUK_RHI_SHADER_DATA_SOURCE_H
#define DUK_RHI_SHADER_DATA_SOURCE_H

#include <duk_rhi/descriptor.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/vertex_layout.h>

#include <duk_hash/data_source.h>

#include <vector>

namespace duk::rhi {

class ShaderDataSource : public duk::hash::DataSource {
public:

    ~ShaderDataSource() override;

    DUK_NO_DISCARD virtual Shader::Module::Mask module_mask() const = 0;

    DUK_NO_DISCARD virtual const std::vector<uint8_t>& shader_module_spir_v_code(Shader::Module::Bits type) const = 0;

    DUK_NO_DISCARD virtual const std::vector<DescriptorSetDescription>& descriptor_set_descriptions() const = 0;

    DUK_NO_DISCARD virtual const VertexLayout& vertex_layout() const = 0;

    DUK_NO_DISCARD bool has_module(Shader::Module::Bits module) const;

    DUK_NO_DISCARD bool has_vertex_module() const;

    DUK_NO_DISCARD bool has_geometry_module() const;

    DUK_NO_DISCARD bool has_tesselation_control_module() const;

    DUK_NO_DISCARD bool has_tesselation_evaluate_module() const;

    DUK_NO_DISCARD bool has_fragment_module() const;

    DUK_NO_DISCARD bool has_compute_module() const;

    DUK_NO_DISCARD bool valid_graphics_shader() const;

    DUK_NO_DISCARD bool valid_compute_shader() const;

};

}

#endif // DUK_RHI_SHADER_DATA_SOURCE_H

