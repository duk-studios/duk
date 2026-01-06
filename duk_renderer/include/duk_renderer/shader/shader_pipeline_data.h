//
// Created by Ricardo on 09/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_DATA_H
#define DUK_RENDERER_SHADER_PIPELINE_DATA_H

#include <duk_renderer/shader/shader_module.h>

#include <duk_serial/json.h>

namespace duk::renderer {

struct PipelineShaderModules {
    ShaderModuleResource vert;
    ShaderModuleResource frag;
};

struct PipelineSettings {
    bool blend;
    bool depth;
    bool invertY;
    duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask;
    uint32_t priority;
};

struct ShaderPipelineData {
    PipelineShaderModules shaders;
    PipelineSettings settings;
};

}// namespace duk::renderer

namespace duk::type {
// clang-format off
template<>
struct Type<duk::renderer::PipelineSettings> : Class<duk::renderer::PipelineSettings,
    Member<"blend", &duk::renderer::PipelineSettings::blend>,
    Member<"depth", &duk::renderer::PipelineSettings::depth>,
    Member<"invertY", &duk::renderer::PipelineSettings::invertY>,
    Member<"cullModeMask", &duk::renderer::PipelineSettings::cullModeMask>,
    Member<"priority", &duk::renderer::PipelineSettings::priority>> {
};

template<>
struct Type<duk::renderer::PipelineShaderModules> : Class<duk::renderer::PipelineShaderModules,
    Member<"vert", &duk::renderer::PipelineShaderModules::vert>,
    Member<"frag", &duk::renderer::PipelineShaderModules::frag>> {
};

template<>
struct Type<duk::renderer::ShaderPipelineData> : Class<duk::renderer::ShaderPipelineData,
    Member<"shaders", &duk::renderer::ShaderPipelineData::shaders>,
    Member<"settings", &duk::renderer::ShaderPipelineData::settings>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_RENDERER_SHADER_PIPELINE_DATA_H
