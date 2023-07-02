/// 15/05/2023
/// shader_data_source.cpp

#include <duk_rhi/pipeline/shader_data_source.h>

namespace duk::rhi {

ShaderDataSource::~ShaderDataSource() = default;

bool ShaderDataSource::has_module(Shader::Module::Bits module) const {
    return module_mask() & module;
}

bool duk::rhi::ShaderDataSource::has_vertex_module() const {
    return has_module(Shader::Module::VERTEX);
}

bool ShaderDataSource::has_geometry_module() const {
    return has_module(Shader::Module::GEOMETRY);
}

bool ShaderDataSource::has_tesselation_control_module() const {
    return has_module(Shader::Module::TESSELLATION_CONTROL);
}

bool ShaderDataSource::has_tesselation_evaluate_module() const {
    return has_module(Shader::Module::TESSELLATION_EVALUATION);
}

bool ShaderDataSource::has_fragment_module() const {
    return has_module(Shader::Module::FRAGMENT);
}

bool ShaderDataSource::has_compute_module() const {
    return has_module(Shader::Module::COMPUTE);
}

bool ShaderDataSource::valid_graphics_shader() const {
    const auto moduleMask = module_mask();
    const auto minimumGraphicsPipelineShader = Shader::Module::VERTEX | Shader::Module::FRAGMENT;
    const bool hasMinimumGraphics = moduleMask & minimumGraphicsPipelineShader;
    const bool hasCompute = moduleMask & Shader::Module::COMPUTE;
    return hasMinimumGraphics && !hasCompute;
}

bool ShaderDataSource::valid_compute_shader() const {
    const auto moduleMask = module_mask();
    const bool hasCompute = moduleMask & Shader::Module::COMPUTE;
    const bool onlyHasCompute = (moduleMask & ~Shader::Module::COMPUTE) == 0;
    return hasCompute && onlyHasCompute;
}

}