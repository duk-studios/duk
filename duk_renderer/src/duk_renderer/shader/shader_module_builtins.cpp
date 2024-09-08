//
// Created by Ricardo on 09/07/2024.
//

#include <duk_renderer/shader/shader_module_builtins.h>
#include <duk_renderer/shader/color/color_shader_data_source.h>
#include <duk_renderer/shader/fullscreen/fullscreen_shader_data_source.h>
#include <duk_renderer/shader/phong/phong_shader_data_source.h>
#include <duk_renderer/shader/text/text_shader_data_source.h>
#include <duk_renderer/shader/image/image_shader_data_source.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<ShaderModule> create_shader_module(const std::vector<uint8_t>& code) {
    ShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.code = code.data();
    shaderModuleCreateInfo.size = code.size();
    return std::make_shared<ShaderModule>(shaderModuleCreateInfo);
}

}// namespace detail

ShaderModuleBuiltins::ShaderModuleBuiltins(const ShaderModuleBuiltinsCreateInfo& shaderModuleBuiltinsCreateInfo) {
    const auto pools = shaderModuleBuiltinsCreateInfo.pools;
    {
        m_color[duk::rhi::ShaderModule::VERTEX] = pools->insert(kColorShaderVertModule, detail::create_shader_module(color_vert_spir_v()));
        m_color[duk::rhi::ShaderModule::FRAGMENT] = pools->insert(kColorShaderFragModule, detail::create_shader_module(color_frag_spir_v()));
    }

    {
        m_phong[duk::rhi::ShaderModule::VERTEX] = pools->insert(kPhongShaderVertModule, detail::create_shader_module(phong_vert_spir_v()));
        m_phong[duk::rhi::ShaderModule::FRAGMENT] = pools->insert(kPhongShaderFragModule, detail::create_shader_module(phong_frag_spir_v()));
    }

    {
        m_text[duk::rhi::ShaderModule::VERTEX] = pools->insert(kTextShaderVertModule, detail::create_shader_module(text_vert_spir_v()));
        m_text[duk::rhi::ShaderModule::FRAGMENT] = pools->insert(kTextShaderFragModule, detail::create_shader_module(text_frag_spir_v()));
    }

    {
        m_fullscreen[duk::rhi::ShaderModule::VERTEX] = pools->insert(kFullscreenShaderVertModule, detail::create_shader_module(fullscreen_vert_spir_v()));
        m_fullscreen[duk::rhi::ShaderModule::FRAGMENT] = pools->insert(kFullscreenShaderFragModule, detail::create_shader_module(fullscreen_frag_spir_v()));
    }

    {
        m_image[duk::rhi::ShaderModule::VERTEX] = pools->insert(kImageShaderVertModule, detail::create_shader_module(image_vert_spir_v()));
        m_image[duk::rhi::ShaderModule::FRAGMENT] = pools->insert(kImageShaderFragModule, detail::create_shader_module(image_frag_spir_v()));
    }
}

duk::resource::Handle<ShaderModule> ShaderModuleBuiltins::color(duk::rhi::ShaderModule::Bits type) const {
    return m_color.at(type);
}

duk::resource::Handle<ShaderModule> ShaderModuleBuiltins::phong(duk::rhi::ShaderModule::Bits type) const {
    return m_phong.at(type);
}

duk::resource::Handle<ShaderModule> ShaderModuleBuiltins::text(duk::rhi::ShaderModule::Bits type) const {
    return m_text.at(type);
}

duk::resource::Handle<ShaderModule> ShaderModuleBuiltins::fullscreen(duk::rhi::ShaderModule::Bits type) const {
    return m_fullscreen.at(type);
}

duk::resource::Handle<ShaderModule> ShaderModuleBuiltins::image(duk::rhi::ShaderModule::Bits type) const {
    return m_image.at(type);
}

}// namespace duk::renderer
