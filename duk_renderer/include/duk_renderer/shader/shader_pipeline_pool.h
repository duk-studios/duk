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
static constexpr duk::resource::Id kColorShaderPipelineId(100'000);
static constexpr duk::resource::Id kPhongShaderPipelineId(100'001);
static constexpr duk::resource::Id kFullscreenShaderPipelineId(100'002);

struct ShaderPipelinePoolCreateInfo {
    Renderer* renderer;
};

class ShaderPipelinePool : public duk::resource::PoolT<ShaderPipelineResource> {
public:

    explicit ShaderPipelinePool(const ShaderPipelinePoolCreateInfo& shaderPipelinePoolCreateInfo);

    ShaderPipelineResource create(const duk::resource::Id& id, const duk::rhi::ShaderDataSource* shaderDataSource, const PipelineSettings& settings);

    ShaderPipelineResource color() const;

    ShaderPipelineResource phong() const;

    ShaderPipelineResource fullscreen() const;

private:
    Renderer* m_renderer;
    ShaderPipelineResource m_color;
    ShaderPipelineResource m_phong;
    ShaderPipelineResource m_fullscreen;
};

}

#endif //DUK_RENDERER_SHADER_PIPELINE_POOL_H
