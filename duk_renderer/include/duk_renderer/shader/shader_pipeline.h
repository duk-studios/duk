//
// Created by Ricardo on 03/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_H
#define DUK_RENDERER_SHADER_PIPELINE_H

#include <duk_renderer/shader/pipeline_cache.h>
#include <duk_renderer/shader/shader_pipeline_data.h>

#include <duk_resource/resource.h>

#include <duk_rhi/command/command_buffer.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/pipeline/shader_data_source.h>

namespace duk::renderer {

class Renderer;
class PipelineCache;

struct ShaderPipelineCreateInfo {
    Renderer* renderer;
    const duk::rhi::ShaderDataSource* shaderDataSource;
    PipelineSettings settings;
};

class ShaderPipeline {
public:
    explicit ShaderPipeline(const ShaderPipelineCreateInfo& shaderPipelineCreateInfo);

    const duk::rhi::DescriptorSetDescription& descriptor_set_description() const;

    void set_blend(bool enable);

    bool blend() const;

    void set_depth(bool enable);

    bool depth() const;

    void set_invert_y(bool invert);

    bool invert_y() const;

    void update(PipelineCache& pipelineCache, duk::rhi::RenderPass* renderPass, const glm::vec2& viewport);

    void bind(duk::rhi::CommandBuffer* commandBuffer) const;

private:
    Renderer* m_renderer;
    std::shared_ptr<duk::rhi::Shader> m_shader;
    std::shared_ptr<duk::rhi::GraphicsPipeline> m_pipeline;
    PipelineState m_state;
    bool m_invertY;
    bool m_dirty;
};

using ShaderPipelineResource = duk::resource::Handle<ShaderPipeline>;

}// namespace duk::renderer

#endif//DUK_RENDERER_SHADER_PIPELINE_H
