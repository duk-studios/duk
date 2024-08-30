//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_builtins.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<ShaderPipeline> create_shader_pipeline(duk::rhi::RHI* rhi, const ShaderPipelineData* shaderPipelineData) {
    ShaderPipelineCreateInfo shaderPipelineCreateInfo = {};
    shaderPipelineCreateInfo.rhi = rhi;
    shaderPipelineCreateInfo.shaderPipelineData = shaderPipelineData;

    return std::make_shared<ShaderPipeline>(shaderPipelineCreateInfo);
}

}// namespace detail

ShaderPipelineBuiltins::ShaderPipelineBuiltins(const ShaderPipelineBuiltinsCreateInfo& shaderPipelinePoolCreateInfo) {
    const auto pools = shaderPipelinePoolCreateInfo.pools;
    const auto rhi = shaderPipelinePoolCreateInfo.rhi;
    const auto shaderModuleBuiltins = shaderPipelinePoolCreateInfo.shaderModuleBuiltins;
    {
        ShaderPipelineData colorShaderPipelineData = {};
        colorShaderPipelineData.settings.blend = false;
        colorShaderPipelineData.settings.depth = true;
        colorShaderPipelineData.settings.invertY = false;
        colorShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        colorShaderPipelineData.shaders.vert = shaderModuleBuiltins->color(duk::rhi::ShaderModule::VERTEX);
        colorShaderPipelineData.shaders.frag = shaderModuleBuiltins->color(duk::rhi::ShaderModule::FRAGMENT);
        m_opaqueColor = pools->insert(kOpaqueColorShaderPipelineId, detail::create_shader_pipeline(rhi, &colorShaderPipelineData));
        colorShaderPipelineData.settings.blend = true;
        colorShaderPipelineData.settings.priority = 1000;
        m_transparentColor = pools->insert(kTransparentColorShaderPipelineId, detail::create_shader_pipeline(rhi, &colorShaderPipelineData));
    }

    {
        ShaderPipelineData phongShaderPipelineData = {};
        phongShaderPipelineData.settings.blend = false;
        phongShaderPipelineData.settings.depth = true;
        phongShaderPipelineData.settings.invertY = false;
        phongShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        phongShaderPipelineData.shaders.vert = shaderModuleBuiltins->phong(duk::rhi::ShaderModule::VERTEX);
        phongShaderPipelineData.shaders.frag = shaderModuleBuiltins->phong(duk::rhi::ShaderModule::FRAGMENT);
        m_opaquePhong = pools->insert(kOpaquePhongShaderPipelineId, detail::create_shader_pipeline(rhi, &phongShaderPipelineData));
        phongShaderPipelineData.settings.blend = true;
        phongShaderPipelineData.settings.priority = 1000;
        m_transparentPhong = pools->insert(kTransparentPhongShaderPipelineId, detail::create_shader_pipeline(rhi, &phongShaderPipelineData));
    }

    {
        ShaderPipelineData fullscreenShaderPipelineData = {};
        fullscreenShaderPipelineData.settings.blend = false;
        fullscreenShaderPipelineData.settings.depth = false;
        fullscreenShaderPipelineData.settings.invertY = true;
        fullscreenShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        fullscreenShaderPipelineData.shaders.vert = shaderModuleBuiltins->fullscreen(duk::rhi::ShaderModule::VERTEX);
        fullscreenShaderPipelineData.shaders.frag = shaderModuleBuiltins->fullscreen(duk::rhi::ShaderModule::FRAGMENT);
        m_fullscreen = pools->insert(kFullscreenShaderPipelineId, detail::create_shader_pipeline(rhi, &fullscreenShaderPipelineData));
    }

    {
        ShaderPipelineData textShaderPipelineData = {};
        textShaderPipelineData.settings.depth = false;
        textShaderPipelineData.settings.blend = true;
        textShaderPipelineData.settings.priority = 2000;
        textShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        textShaderPipelineData.shaders.vert = shaderModuleBuiltins->text(duk::rhi::ShaderModule::VERTEX);
        textShaderPipelineData.shaders.frag = shaderModuleBuiltins->text(duk::rhi::ShaderModule::FRAGMENT);
        m_text = pools->insert(kTextShaderPipelineId, detail::create_shader_pipeline(rhi, &textShaderPipelineData));
    }

    {
        ShaderPipelineData imageShaderPipelineData = {};
        imageShaderPipelineData.settings.depth = false;
        imageShaderPipelineData.settings.blend = true;
        imageShaderPipelineData.settings.priority = 2000;
        imageShaderPipelineData.settings.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        imageShaderPipelineData.shaders.vert = shaderModuleBuiltins->image(duk::rhi::ShaderModule::VERTEX);
        imageShaderPipelineData.shaders.frag = shaderModuleBuiltins->image(duk::rhi::ShaderModule::FRAGMENT);
        m_image = pools->insert(kImageShaderPipelineId, detail::create_shader_pipeline(rhi, &imageShaderPipelineData));
    }
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineBuiltins::opaque_color() const {
    return m_opaqueColor;
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineBuiltins::transparent_color() const {
    return m_transparentColor;
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineBuiltins::opaque_phong() const {
    return m_opaquePhong;
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineBuiltins::transparent_phong() const {
    return m_transparentPhong;
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineBuiltins::fullscreen() const {
    return m_fullscreen;
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineBuiltins::text() const {
    return m_text;
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineBuiltins::image() const {
    return m_image;
}

}// namespace duk::renderer
