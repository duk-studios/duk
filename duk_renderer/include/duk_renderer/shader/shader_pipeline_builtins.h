//
// Created by Ricardo on 09/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_BUILTINS_H
#define DUK_RENDERER_SHADER_PIPELINE_BUILTINS_H

#include <duk_renderer/shader/shader_pipeline.h>
#include <duk_renderer/shader/shader_pipeline_data.h>
#include <duk_renderer/shader/shader_module_builtins.h>
#include <duk_resource/pool.h>

#include <duk_rhi/rhi.h>

namespace duk::renderer {

// 100,000 - 101,000 - reserved for built-in shaders
static constexpr duk::resource::Id kOpaqueColorShaderPipelineId(100'000);
static constexpr duk::resource::Id kTransparentColorShaderPipelineId(100'001);
static constexpr duk::resource::Id kOpaquePhongShaderPipelineId(100'002);
static constexpr duk::resource::Id kTransparentPhongShaderPipelineId(100'003);
static constexpr duk::resource::Id kFullscreenShaderPipelineId(100'004);
static constexpr duk::resource::Id kTextShaderPipelineId(100'005);
static constexpr duk::resource::Id kImageShaderPipelineId(100'007);

struct ShaderPipelineBuiltinsCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    ShaderModuleBuiltins* shaderModuleBuiltins;
    duk::resource::Pools* pools;
};

class ShaderPipelineBuiltins {
public:
    explicit ShaderPipelineBuiltins(const ShaderPipelineBuiltinsCreateInfo& shaderPipelineBuiltinsCreateInfo);

    duk::resource::Handle<ShaderPipeline> opaque_color() const;

    duk::resource::Handle<ShaderPipeline> transparent_color() const;

    duk::resource::Handle<ShaderPipeline> opaque_phong() const;

    duk::resource::Handle<ShaderPipeline> transparent_phong() const;

    duk::resource::Handle<ShaderPipeline> fullscreen() const;

    duk::resource::Handle<ShaderPipeline> text() const;

    duk::resource::Handle<ShaderPipeline> image() const;

private:
    duk::resource::Handle<ShaderPipeline> m_opaqueColor;
    duk::resource::Handle<ShaderPipeline> m_transparentColor;
    duk::resource::Handle<ShaderPipeline> m_opaquePhong;
    duk::resource::Handle<ShaderPipeline> m_transparentPhong;
    duk::resource::Handle<ShaderPipeline> m_fullscreen;
    duk::resource::Handle<ShaderPipeline> m_text;
    duk::resource::Handle<ShaderPipeline> m_image;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SHADER_PIPELINE_BUILTINS_H
