//
// Created by Ricardo on 19/06/2024.
//

#ifndef DUK_RENDERER_SHADER_MODULE_POOL_H
#define DUK_RENDERER_SHADER_MODULE_POOL_H

#include <duk_renderer/shader/shader_module.h>
#include <duk_resource/pool.h>
#include <duk_rhi/pipeline/shader.h>

#include <array>

namespace duk::renderer {

// 110,000 - 111,000 - reserved for built-in shader modules
static constexpr duk::resource::Id kColorShaderVertModule(110'000);
static constexpr duk::resource::Id kColorShaderFragModule(110'001);
static constexpr duk::resource::Id kPhongShaderVertModule(110'002);
static constexpr duk::resource::Id kPhongShaderFragModule(110'003);
static constexpr duk::resource::Id kTextShaderVertModule(110'004);
static constexpr duk::resource::Id kTextShaderFragModule(110'005);
static constexpr duk::resource::Id kFullscreenShaderVertModule(110'006);
static constexpr duk::resource::Id kFullscreenShaderFragModule(110'007);

class Renderer;

class ShaderModulePool : public duk::resource::PoolT<ShaderModuleResource> {
public:
    ShaderModulePool();

    ShaderModuleResource color(duk::rhi::ShaderModule::Bits type);

    ShaderModuleResource phong(duk::rhi::ShaderModule::Bits type);

    ShaderModuleResource text(duk::rhi::ShaderModule::Bits type);

    ShaderModuleResource fullscreen(duk::rhi::ShaderModule::Bits type);

private:
    std::unordered_map<duk::rhi::ShaderModule::Bits, ShaderModuleResource> m_color;
    std::unordered_map<duk::rhi::ShaderModule::Bits, ShaderModuleResource> m_phong;
    std::unordered_map<duk::rhi::ShaderModule::Bits, ShaderModuleResource> m_text;
    std::unordered_map<duk::rhi::ShaderModule::Bits, ShaderModuleResource> m_fullscreen;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SHADER_MODULE_POOL_H
