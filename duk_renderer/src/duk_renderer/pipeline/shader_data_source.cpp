/// 15/05/2023
/// shader_data_source.cpp

#include <duk_renderer/pipeline/shader_data_source.h>

namespace duk::renderer {

ShaderDataSource::~ShaderDataSource() = default;

bool duk::renderer::ShaderDataSource::has_vertex_module() const {
    return module_mask() & Shader::ModuleType::VERTEX;
}

bool ShaderDataSource::has_geometry_module() const {
    return module_mask() & Shader::ModuleType::GEOMETRY;
}

bool ShaderDataSource::has_tesselation_control_module() const {
    return module_mask() & Shader::ModuleType::TESSELATION_CTRL;
}

bool ShaderDataSource::has_tesselation_evaluate_module() const {
    return module_mask() & Shader::ModuleType::TESSELATION_EVAL;
}

bool ShaderDataSource::has_fragment_module() const {
    return module_mask() & Shader::ModuleType::FRAGMENT;
}

bool ShaderDataSource::has_compute_module() const {
    return module_mask() & Shader::ModuleType::COMPUTE;
}

bool ShaderDataSource::valid_graphics_shader() const {
    const auto moduleMask = module_mask();
    const auto minimumGraphicsPipelineShader = Shader::ModuleType::VERTEX | Shader::ModuleType::FRAGMENT;
    const bool hasMinimumGraphics = moduleMask & minimumGraphicsPipelineShader;
    const bool hasCompute = moduleMask & Shader::ModuleType::COMPUTE;
    return hasMinimumGraphics && !hasCompute;
}

bool ShaderDataSource::valid_compute_shader() const {
    const auto moduleMask = module_mask();
    const bool hasCompute = moduleMask & Shader::ModuleType::COMPUTE;
    const bool onlyHasCompute = (moduleMask & ~Shader::ModuleType::COMPUTE) == 0;
    return hasCompute && onlyHasCompute;
}

}