//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_pool.h>
#include <duk_renderer/shader/color/color_shader_data_source.h>
#include <duk_renderer/shader/phong/phong_shader_data_source.h>
#include <duk_renderer/shader/fullscreen/fullscreen_shader_data_source.h>
#include <duk_renderer/shader/text/text_shader_data_source.h>

namespace duk::renderer {

ShaderPipelinePool::ShaderPipelinePool(const ShaderPipelinePoolCreateInfo& shaderPipelinePoolCreateInfo)
    : m_renderer(shaderPipelinePoolCreateInfo.renderer) {
    {
        ColorShaderDataSource colorShaderDataSource = {};
        PipelineSettings settings = {};
        settings.blend = false;
        settings.depth = true;
        settings.invertY = false;
        m_opaqueColor = create(kOpaqueColorShaderPipelineId, &colorShaderDataSource, settings);
        settings.blend = true;
        m_transparentColor = create(kTransparentColorShaderPipelineId, &colorShaderDataSource, settings);
    }

    {
        PhongShaderDataSource phongShaderDataSource = {};
        PipelineSettings settings = {};
        settings.blend = false;
        settings.depth = true;
        settings.invertY = false;
        m_opaquePhong = create(kOpaquePhongShaderPipelineId, &phongShaderDataSource, settings);
        settings.blend = true;
        m_transparentPhong = create(kTransparentPhongShaderPipelineId, &phongShaderDataSource, settings);
    }

    {
        FullscreenShaderDataSource fullscreenShaderDataSource = {};
        PipelineSettings settings = {};
        settings.blend = false;
        settings.depth = false;
        settings.invertY = true;
        m_fullscreen = create(kFullscreenShaderPipelineId, &fullscreenShaderDataSource, settings);
    }

    {
        TextShaderDataSource textShaderDataSource = {};
        PipelineSettings settings = {};
        settings.blend = true;
        settings.depth = true;
        settings.invertY = false;
        m_text = create(kTextShaderPipelineId, &textShaderDataSource, settings);
    }
}

ShaderPipelineResource ShaderPipelinePool::create(const duk::resource::Id& id, const duk::rhi::ShaderDataSource* shaderDataSource, const PipelineSettings& settings) {
    ShaderPipelineCreateInfo shaderPipelineCreateInfo = {};
    shaderPipelineCreateInfo.renderer = m_renderer;
    shaderPipelineCreateInfo.shaderDataSource = shaderDataSource;
    shaderPipelineCreateInfo.settings = settings;

    return insert(id, std::make_shared<ShaderPipeline>(shaderPipelineCreateInfo));
}

ShaderPipelineResource ShaderPipelinePool::opaque_color() const {
    return m_opaqueColor;
}

ShaderPipelineResource ShaderPipelinePool::transparent_color() const {
    return m_transparentColor;
}

ShaderPipelineResource ShaderPipelinePool::opaque_phong() const {
    return m_opaquePhong;
}

ShaderPipelineResource ShaderPipelinePool::transparent_phong() const {
    return m_transparentPhong;
}

ShaderPipelineResource ShaderPipelinePool::fullscreen() const {
    return m_fullscreen;
}

ShaderPipelineResource ShaderPipelinePool::text() const {
    return m_text;
}

}// namespace duk::renderer
