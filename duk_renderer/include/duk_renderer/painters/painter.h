/// 08/09/2023
/// painter.h

#ifndef DUK_RENDERER_PAINTER_H
#define DUK_RENDERER_PAINTER_H

#include <duk_renderer/canvas.h>

#include <duk_rhi/descriptor_set.h>
#include <duk_rhi/command/command_buffer.h>
#include <duk_rhi/rhi.h>


namespace duk::renderer {

class Mesh;
class Palette;

class Painter {
protected:

    explicit Painter(duk::rhi::RHI* rhi);

public:

    virtual ~Painter();

    struct PaintParams {
        Canvas* canvas;
        Mesh* mesh;
        Palette* palette;
        size_t instanceCount;
        size_t firstInstance;
    };

    void paint(duk::rhi::CommandBuffer* commandBuffer, const PaintParams& params);

    DUK_NO_DISCARD virtual const duk::rhi::ShaderDataSource* shader_data_source() const = 0;

protected:

    duk::rhi::GraphicsPipeline* pipeline_for_params(const Painter::PaintParams& params);

    // should be called by derived classes after loading their data sources
    void init_shader(const rhi::ShaderDataSource* shaderDataSource);

protected:
    duk::rhi::RHI* m_rhi;
private:
    std::shared_ptr<duk::rhi::Shader> m_shader;

    struct PipelineEntry {
        std::shared_ptr<duk::rhi::GraphicsPipeline> pipeline;
        int framesUnused;
    };

    std::unordered_map<duk::hash::Hash, PipelineEntry> m_pipelines;
};

}

#endif // DUK_RENDERER_PAINTER_H

