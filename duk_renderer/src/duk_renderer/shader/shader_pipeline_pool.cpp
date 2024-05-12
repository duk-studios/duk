//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_pool.h>
#include <duk_renderer/shader/color/color_shader_data_source.h>
#include <duk_renderer/shader/phong/phong_shader_data_source.h>
#include <duk_renderer/shader/fullscreen/fullscreen_shader_data_source.h>


namespace duk::renderer {

ShaderPipelinePool::ShaderPipelinePool(const ShaderPipelinePoolCreateInfo& shaderPipelinePoolCreateInfo)
    : m_renderer(shaderPipelinePoolCreateInfo.renderer) {

    {
        ColorShaderDataSource colorShaderDataSource = {};
        PipelineSettings settings = {};
        settings.blend = false;
        settings.depth = true;
        settings.invertY = false;
        m_color = create(kColorShaderPipelineId, &colorShaderDataSource, settings);
    }

    {
        PhongShaderDataSource phongShaderDataSource = {};
        PipelineSettings settings = {};
        settings.blend = false;
        settings.depth = true;
        settings.invertY = false;
        m_phong = create(kPhongShaderPipelineId, &phongShaderDataSource, settings);
    }

    {
        FullscreenShaderDataSource fullscreenShaderDataSource = {};
        PipelineSettings settings = {};
        settings.blend = false;
        settings.depth = false;
        settings.invertY = true;
        m_fullscreen = create(kFullscreenShaderPipelineId, &fullscreenShaderDataSource, settings);
    }
}

ShaderPipelineResource ShaderPipelinePool::create(const duk::resource::Id& id, const duk::rhi::ShaderDataSource* shaderDataSource, const PipelineSettings& settings) {
    ShaderPipelineCreateInfo shaderPipelineCreateInfo = {};
    shaderPipelineCreateInfo.renderer = m_renderer;
    shaderPipelineCreateInfo.shaderDataSource = shaderDataSource;
    shaderPipelineCreateInfo.settings = settings;

    return insert(id, std::make_shared<ShaderPipeline>(shaderPipelineCreateInfo));
}

ShaderPipelineResource ShaderPipelinePool::color() const {
    return m_color;
}

ShaderPipelineResource ShaderPipelinePool::phong() const {
    return m_phong;
}

ShaderPipelineResource ShaderPipelinePool::fullscreen() const {
    return m_fullscreen;
}
}
