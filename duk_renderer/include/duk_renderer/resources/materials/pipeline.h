/// 08/09/2023
/// pipeline.h

#ifndef DUK_RENDERER_PIPELINE_H
#define DUK_RENDERER_PIPELINE_H

#include <duk_renderer/resources/materials/draw_entry.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class Renderer;
class Brush;
class Material;
class GlobalDescriptors;

struct PipelineCreateInfo {
    Renderer* renderer;
    const duk::rhi::ShaderDataSource* shaderDataSource;
};

class Pipeline {
public:
    explicit Pipeline(const PipelineCreateInfo& pipelineCreateInfo);

    void update(const DrawParams& params);

    void use(duk::rhi::CommandBuffer* commandBuffer);

    void update_shader(duk::rhi::ShaderDataSource* shaderDataSource);

    void invert_y(bool invert);

    void cull_mode_mask(duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask);

private:
    class PipelineCache {
    public:
        PipelineCache(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource);

        void update_shader(const duk::rhi::ShaderDataSource* shaderDataSource);

        duk::rhi::GraphicsPipeline* find_pipeline_for_params(const DrawParams& params);

        void invert_y(bool invert);

        void cull_mode_mask(duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask);

        void clear_unused_pipelines();

    private:
        duk::hash::Hash hash_for_params(const DrawParams& params) const;

        duk::rhi::GraphicsPipeline::Viewport viewport_for_params(const DrawParams& params);

    private:
        struct PipelineEntry {
            std::shared_ptr<duk::rhi::GraphicsPipeline> pipeline;
            int framesUnused;
        };

    private:
        std::unordered_map<duk::hash::Hash, PipelineEntry> m_pipelines;
        Renderer* m_renderer;
        std::shared_ptr<duk::rhi::Shader> m_shader;
        duk::rhi::GraphicsPipeline::CullMode::Mask m_cullModeMask;
        bool m_invertY;
    };

private:
    Renderer* m_renderer;
    PipelineCache m_pipelineCache;
    duk::rhi::GraphicsPipeline* m_currentPipeline;
    duk::event::Listener m_listener;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_PIPELINE_H
