//
// Created by Ricardo on 09/07/2024.
//

#ifndef SHADER_MODULE_BUILTINS_H
#define SHADER_MODULE_BUILTINS_H

#include <duk_renderer/shader/shader_module.h>

#include <duk_resource/handle.h>
#include <duk_resource/pool.h>

#include <duk_rhi/pipeline/shader.h>

#include <unordered_map>

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
static constexpr duk::resource::Id kImageShaderVertModule(110'008);
static constexpr duk::resource::Id kImageShaderFragModule(110'009);

struct ShaderModuleBuiltinsCreateInfo {
    duk::resource::Pools* pools;
};

class ShaderModuleBuiltins {
public:
    ShaderModuleBuiltins(const ShaderModuleBuiltinsCreateInfo& shaderModuleBuiltinsCreateInfo);

    duk::resource::Handle<ShaderModule> color(duk::rhi::ShaderModule::Bits type) const;

    duk::resource::Handle<ShaderModule> phong(duk::rhi::ShaderModule::Bits type) const;

    duk::resource::Handle<ShaderModule> text(duk::rhi::ShaderModule::Bits type) const;

    duk::resource::Handle<ShaderModule> fullscreen(duk::rhi::ShaderModule::Bits type) const;

    duk::resource::Handle<ShaderModule> image(duk::rhi::ShaderModule::Bits type) const;

private:
    std::unordered_map<duk::rhi::ShaderModule::Bits, duk::resource::Handle<ShaderModule>> m_color;
    std::unordered_map<duk::rhi::ShaderModule::Bits, duk::resource::Handle<ShaderModule>> m_phong;
    std::unordered_map<duk::rhi::ShaderModule::Bits, duk::resource::Handle<ShaderModule>> m_text;
    std::unordered_map<duk::rhi::ShaderModule::Bits, duk::resource::Handle<ShaderModule>> m_fullscreen;
    std::unordered_map<duk::rhi::ShaderModule::Bits, duk::resource::Handle<ShaderModule>> m_image;
};

}// namespace duk::renderer

#endif//SHADER_MODULE_BUILTINS_H
