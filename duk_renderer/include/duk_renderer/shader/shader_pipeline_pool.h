//
// Created by Ricardo on 09/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_POOL_H
#define DUK_RENDERER_SHADER_PIPELINE_POOL_H

#include <duk_renderer/shader/shader_pipeline.h>
#include <duk_renderer/shader/shader_pipeline_data.h>

#include <duk_resource/pool.h>

namespace duk::renderer {

// 100,000 - 101,000 - reserved for built-in shaders
static constexpr duk::resource::Id kOpaqueColorShaderPipelineId(100'000);
static constexpr duk::resource::Id kTransparentColorShaderPipelineId(100'001);
static constexpr duk::resource::Id kOpaquePhongShaderPipelineId(100'002);
static constexpr duk::resource::Id kTransparentPhongShaderPipelineId(100'003);
static constexpr duk::resource::Id kFullscreenShaderPipelineId(100'004);
static constexpr duk::resource::Id kTextShaderPipelineId(100'005);

struct ShaderPipelinePoolCreateInfo {
    Renderer* renderer;
};

class ShaderPipelinePool : public duk::resource::PoolT<ShaderPipelineResource> {
public:

    explicit ShaderPipelinePool(const ShaderPipelinePoolCreateInfo& shaderPipelinePoolCreateInfo);

    ShaderPipelineResource create(const duk::resource::Id& id, const duk::rhi::ShaderDataSource* shaderDataSource, const PipelineSettings& settings);

    ShaderPipelineResource opaque_color() const;

    ShaderPipelineResource transparent_color() const;

    ShaderPipelineResource opaque_phong() const;

    ShaderPipelineResource transparent_phong() const;

    ShaderPipelineResource fullscreen() const;

    ShaderPipelineResource text() const;

private:
    Renderer* m_renderer;
    ShaderPipelineResource m_opaqueColor;
    ShaderPipelineResource m_transparentColor;
    ShaderPipelineResource m_opaquePhong;
    ShaderPipelineResource m_transparentPhong;
    ShaderPipelineResource m_fullscreen;
    ShaderPipelineResource m_text;
};

}

#endif //DUK_RENDERER_SHADER_PIPELINE_POOL_H
