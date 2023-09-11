/// 08/09/2023
/// painter.cpp

#include <duk_renderer/painters/painter.h>

namespace duk::renderer {


Painter::Painter(duk::rhi::RHI* rhi) : m_rhi(rhi) {
}

Painter::~Painter() = default;


duk::rhi::GraphicsPipeline* Painter::pipeline_for_params(const Painter::PaintParams& params) {

    duk::rhi::RHI::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.viewport.extent = {params.canvas->width(), params.canvas->height()};
    pipelineCreateInfo.viewport.maxDepth = 1.0f;
    pipelineCreateInfo.viewport.minDepth = 0.0f;
    pipelineCreateInfo.scissor.extent = pipelineCreateInfo.viewport.extent;
    pipelineCreateInfo.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
    pipelineCreateInfo.fillMode = duk::rhi::GraphicsPipeline::FillMode::FILL;
    pipelineCreateInfo.topology = duk::rhi::GraphicsPipeline::Topology::TRIANGLE_LIST;
    pipelineCreateInfo.shader = m_shader.get();
    pipelineCreateInfo.renderPass = params.canvas->render_pass();
    pipelineCreateInfo.depthTesting = true;

    const auto hash = duk::rhi::GraphicsPipeline::hash_of(pipelineCreateInfo.viewport,
                                                          pipelineCreateInfo.scissor,
                                                          pipelineCreateInfo.blend,
                                                          pipelineCreateInfo.shader,
                                                          pipelineCreateInfo.renderPass,
                                                          pipelineCreateInfo.cullModeMask,
                                                          pipelineCreateInfo.depthTesting);

    auto it = m_pipelines.find(hash);
    if (it == m_pipelines.end()) {
        auto expectedPipeline = m_rhi->create_graphics_pipeline(pipelineCreateInfo);
        if (!expectedPipeline) {
            throw std::runtime_error("failed to create Pipeline: " + expectedPipeline.error().description());
        }

        PipelineEntry entry = {};
        entry.pipeline = std::move(expectedPipeline.value());
        entry.framesUnused = 0;

        auto result = m_pipelines.emplace(hash, entry);
        if (!result.second) {
            throw std::runtime_error("failed to insert Pipeline into cache");
        }

        return result.first->second.pipeline.get();
    }

    it->second.framesUnused = 0;

    return it->second.pipeline.get();
}

void Painter::init_shader(const rhi::ShaderDataSource* shaderDataSource) {

    duk::rhi::RHI::ShaderCreateInfo colorShaderCreateInfo = {};
    colorShaderCreateInfo.shaderDataSource = shaderDataSource;

    auto expectedColorShader = m_rhi->create_shader(colorShaderCreateInfo);

    if (!expectedColorShader) {
        throw std::runtime_error("failed to init Shader: " + expectedColorShader.error().description());
    }

    m_shader = std::move(expectedColorShader.value());

}

}