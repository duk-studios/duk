//
// Created by Ricardo on 19/06/2024.
//

#include <duk_renderer/shader/shader_module_handler.h>

#include <duk_renderer/shader/color/color_shader_data_source.h>
#include <duk_renderer/shader/fullscreen/fullscreen_shader_data_source.h>
#include <duk_renderer/shader/phong/phong_shader_data_source.h>
#include <duk_renderer/shader/text/text_shader_data_source.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<ShaderModule> create_shader_module(const std::vector<uint8_t>& code) {
    ShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.code = code.data();
    shaderModuleCreateInfo.size = code.size();
    return std::make_shared<ShaderModule>(shaderModuleCreateInfo);
}

}// namespace detail

ShaderModulePool::ShaderModulePool() {
    {
        m_color[duk::rhi::ShaderModule::VERTEX] = insert(kColorShaderVertModule, detail::create_shader_module(color_vert_spir_v()));
        m_color[duk::rhi::ShaderModule::FRAGMENT] = insert(kColorShaderFragModule, detail::create_shader_module(color_frag_spir_v()));
    }

    {
        m_phong[duk::rhi::ShaderModule::VERTEX] = insert(kPhongShaderVertModule, detail::create_shader_module(phong_vert_spir_v()));
        m_phong[duk::rhi::ShaderModule::FRAGMENT] = insert(kPhongShaderFragModule, detail::create_shader_module(phong_frag_spir_v()));
    }

    {
        m_text[duk::rhi::ShaderModule::VERTEX] = insert(kTextShaderVertModule, detail::create_shader_module(text_vert_spir_v()));
        m_text[duk::rhi::ShaderModule::FRAGMENT] = insert(kTextShaderFragModule, detail::create_shader_module(text_frag_spir_v()));
    }

    {
        m_fullscreen[duk::rhi::ShaderModule::VERTEX] = insert(kFullscreenShaderVertModule, detail::create_shader_module(fullscreen_vert_spir_v()));
        m_fullscreen[duk::rhi::ShaderModule::FRAGMENT] = insert(kFullscreenShaderFragModule, detail::create_shader_module(fullscreen_frag_spir_v()));
    }
}

ShaderModuleResource ShaderModulePool::color(duk::rhi::ShaderModule::Bits type) {
    return m_color[type];
}

ShaderModuleResource ShaderModulePool::phong(duk::rhi::ShaderModule::Bits type) {
    return m_phong[type];
}

ShaderModuleResource ShaderModulePool::text(duk::rhi::ShaderModule::Bits type) {
    return m_text[type];
}

ShaderModuleResource ShaderModulePool::fullscreen(duk::rhi::ShaderModule::Bits type) {
    return m_fullscreen[type];
}

}// namespace duk::renderer
