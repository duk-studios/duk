/// 08/09/2023
/// painter.cpp

#include <duk_renderer/painters/painter.h>
#include <duk_renderer/painters/palette.h>
#include <duk_renderer/brushes/brush.h>

namespace duk::renderer {

static constexpr int kDeleteUnusedPipelineAfterFrames = 512;

Painter::Painter(duk::rhi::RHI* rhi) :
    m_rhi(rhi) {
}

Painter::~Painter() = default;

void Painter::paint(duk::rhi::CommandBuffer* commandBuffer, const Painter::PaintParams& params) {
    commandBuffer->bind_graphics_pipeline(find_pipeline_for_params(params));

    Palette::ApplyParams paletteApplyParams = {};
    paletteApplyParams.globalDescriptors = params.globalDescriptors;

    params.palette->apply(commandBuffer, paletteApplyParams);

    params.brush->draw(commandBuffer, params.instanceCount, params.firstInstance);
}

void Painter::clear_unused_pipelines() {
    std::vector<duk::hash::Hash> hashesToDelete;
    hashesToDelete.reserve(m_pipelines.size());

    for (auto& [hash, entry] : m_pipelines) {
        entry.framesUnused++;

        if (entry.framesUnused > kDeleteUnusedPipelineAfterFrames) {
            hashesToDelete.push_back(hash);
        }
    }

    for (auto hash : hashesToDelete) {
        m_pipelines.erase(hash);
    }
}

duk::rhi::GraphicsPipeline* Painter::find_pipeline_for_params(const Painter::PaintParams& params) {

    const auto hash = hash_for_params(params);

    auto it = m_pipelines.find(hash);
    if (it == m_pipelines.end()) {

        duk::rhi::RHI::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.viewport = viewport_for_params(params);
        pipelineCreateInfo.scissor.extent.x = params.outputWidth;
        pipelineCreateInfo.scissor.extent.y = params.outputHeight;
        pipelineCreateInfo.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
        pipelineCreateInfo.fillMode = duk::rhi::GraphicsPipeline::FillMode::FILL;
        pipelineCreateInfo.topology = duk::rhi::GraphicsPipeline::Topology::TRIANGLE_LIST;
        pipelineCreateInfo.shader = m_shader.get();
        pipelineCreateInfo.renderPass = params.renderPass;
        pipelineCreateInfo.depthTesting = true;

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

duk::hash::Hash Painter::hash_for_params(const Painter::PaintParams& params) const {
    hash::Hash hash = 0;
    hash::hash_combine(hash, params.renderPass);
    hash::hash_combine(hash, params.outputWidth);
    hash::hash_combine(hash, params.outputHeight);
    return hash;
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

duk::rhi::GraphicsPipeline::Viewport Painter::viewport_for_params(const Painter::PaintParams& params) {
    duk::rhi::GraphicsPipeline::Viewport viewport = {};
    viewport.extent = {params.outputWidth, params.outputHeight};
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;
    return viewport;
}

}