/// 08/09/2023
/// painter.h

#ifndef DUK_RENDERER_PAINTER_H
#define DUK_RENDERER_PAINTER_H

#include <duk_renderer/materials/paint_entry.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class Renderer;
class Brush;
class Material;
class GlobalDescriptors;

struct PainterCreateInfo {
    Renderer* renderer;
    const duk::rhi::ShaderDataSource* shaderDataSource;
};

class Painter {
public:
    Painter(const PainterCreateInfo& painterCreateInfo);

    void use(duk::rhi::CommandBuffer* commandBuffer, const PaintParams& params);

    void update_shader(duk::rhi::ShaderDataSource* shaderDataSource);

    void invert_y(bool invert);

    void clear_unused_pipelines();

private:
    class PipelineCache {
    public:
        PipelineCache(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource);

        void update_shader(const duk::rhi::ShaderDataSource* shaderDataSource);

        duk::rhi::GraphicsPipeline* find_pipeline_for_params(const PaintParams& params);

        void invert_y(bool invert);

        void clear_unused_pipelines();

    private:
        duk::hash::Hash hash_for_params(const PaintParams& params) const;

        duk::rhi::GraphicsPipeline::Viewport viewport_for_params(const PaintParams& params);

    private:
        struct PipelineEntry {
            std::shared_ptr<duk::rhi::GraphicsPipeline> pipeline;
            int framesUnused;
        };

    private:
        std::unordered_map<duk::hash::Hash, PipelineEntry> m_pipelines;
        Renderer* m_renderer;
        std::shared_ptr<duk::rhi::Shader> m_shader;
        bool m_invertY;
    };

private:
    Renderer* m_renderer;
    PipelineCache m_pipelineCache;
    duk::events::EventListener m_listener;
};

}

#endif // DUK_RENDERER_PAINTER_H

