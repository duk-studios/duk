/// 15/05/2023
/// shader_data_source.h

#ifndef DUK_RENDERER_SHADER_DATA_SOURCE_H
#define DUK_RENDERER_SHADER_DATA_SOURCE_H

#include <duk_renderer/data_source.h>
#include <duk_renderer/pipeline/shader.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::renderer {

class ShaderDataSource : public DataSource {
public:

    virtual ~ShaderDataSource();

    DUK_NO_DISCARD virtual Shader::ModuleMask module_mask() const = 0;

    DUK_NO_DISCARD virtual const std::vector<uint8_t>& shader_module_spir_v_code(Shader::ModuleType::Bits type) const = 0;

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

#endif // DUK_RENDERER_SHADER_DATA_SOURCE_H

