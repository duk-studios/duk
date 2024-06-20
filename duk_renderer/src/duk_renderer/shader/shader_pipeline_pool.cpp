//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_pool.h>

namespace duk::renderer {

ShaderPipelinePool::ShaderPipelinePool(const ShaderPipelinePoolCreateInfo& shaderPipelinePoolCreateInfo)
    : m_renderer(shaderPipelinePoolCreateInfo.renderer) {
    auto shaderModulePool = shaderPipelinePoolCreateInfo.shaderModulePool;
    {
        ShaderPipelineData colorShaderPipelineData = {};
        colorShaderPipelineData.settings.blend = false;
        colorShaderPipelineData.settings.depth = true;
        colorShaderPipelineData.settings.invertY = false;
        colorShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        colorShaderPipelineData.shaders.vert = shaderModulePool->color(duk::rhi::ShaderModule::VERTEX);
        colorShaderPipelineData.shaders.frag = shaderModulePool->color(duk::rhi::ShaderModule::FRAGMENT);
        m_opaqueColor = create(kOpaqueColorShaderPipelineId, &colorShaderPipelineData);
        colorShaderPipelineData.settings.blend = true;
        colorShaderPipelineData.settings.priority = 1000;
        m_transparentColor = create(kTransparentColorShaderPipelineId, &colorShaderPipelineData);
    }

    {
        ShaderPipelineData phongShaderPipelineData = {};
        phongShaderPipelineData.settings.blend = false;
        phongShaderPipelineData.settings.depth = true;
        phongShaderPipelineData.settings.invertY = false;
        phongShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        phongShaderPipelineData.shaders.vert = shaderModulePool->phong(duk::rhi::ShaderModule::VERTEX);
        phongShaderPipelineData.shaders.frag = shaderModulePool->phong(duk::rhi::ShaderModule::FRAGMENT);
        m_opaquePhong = create(kOpaquePhongShaderPipelineId, &phongShaderPipelineData);
        phongShaderPipelineData.settings.blend = true;
        phongShaderPipelineData.settings.priority = 1000;
        m_transparentPhong = create(kTransparentPhongShaderPipelineId, &phongShaderPipelineData);
    }

    {
        ShaderPipelineData fullscreenShaderPipelineData = {};
        fullscreenShaderPipelineData.settings.blend = false;
        fullscreenShaderPipelineData.settings.depth = false;
        fullscreenShaderPipelineData.settings.invertY = true;
        fullscreenShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        fullscreenShaderPipelineData.shaders.vert = shaderModulePool->fullscreen(duk::rhi::ShaderModule::VERTEX);
        fullscreenShaderPipelineData.shaders.frag = shaderModulePool->fullscreen(duk::rhi::ShaderModule::FRAGMENT);
        m_fullscreen = create(kFullscreenShaderPipelineId, &fullscreenShaderPipelineData);
    }

    {
        ShaderPipelineData textShaderPipelineData = {};
        textShaderPipelineData.settings.depth = false;
        textShaderPipelineData.settings.blend = true;
        textShaderPipelineData.settings.priority = 2000;
        textShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        textShaderPipelineData.shaders.vert = shaderModulePool->text(duk::rhi::ShaderModule::VERTEX);
        textShaderPipelineData.shaders.frag = shaderModulePool->text(duk::rhi::ShaderModule::FRAGMENT);
        m_text = create(kTextShaderPipelineId, &textShaderPipelineData);
    }
}

ShaderPipelineResource ShaderPipelinePool::create(const duk::resource::Id& id, const ShaderPipelineData* shaderPipelineData) {
    ShaderPipelineCreateInfo shaderPipelineCreateInfo = {};
    shaderPipelineCreateInfo.renderer = m_renderer;
    shaderPipelineCreateInfo.shaderPipelineData = shaderPipelineData;

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
