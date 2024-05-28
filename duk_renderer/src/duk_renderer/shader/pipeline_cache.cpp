//
// Created by Ricardo on 04/05/2024.
//

#include <duk_renderer/renderer.h>
#include <duk_renderer/shader/pipeline_cache.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

// duk::rhi::GraphicsPipeline::Viewport PipelineCache::viewport_for_params(const DrawParams& params) {
//     duk::rhi::GraphicsPipeline::Viewport viewport = {};
//     viewport.extent = {params.outputWidth, params.outputHeight};
//     viewport.maxDepth = 1.0f;
//     viewport.minDepth = 0.0f;
//     if (m_invertY) {
//         viewport.offset.y = viewport.extent.y;
//         viewport.extent.y = -viewport.extent.y;
//     }
//     return viewport;
// }
//
// void PipelineCache::blend(bool enabled) {
//     if (enabled) {
//         m_blend.enabled = true;
//         m_blend.colorBlendOp = rhi::GraphicsPipeline::Blend::Operator::ADD;
//         m_blend.srcColorBlendFactor = rhi::GraphicsPipeline::Blend::Factor::SRC_ALPHA;
//         m_blend.dstColorBlendFactor = rhi::GraphicsPipeline::Blend::Factor::ONE_MINUS_SRC_ALPHA;
//         m_blend.alphaBlendOp = rhi::GraphicsPipeline::Blend::Operator::ADD;
//         m_blend.srcAlphaBlendFactor = rhi::GraphicsPipeline::Blend::Factor::ONE;
//         m_blend.dstAlphaBlendFactor = rhi::GraphicsPipeline::Blend::Factor::ZERO;
//     } else {
//         m_blend.enabled = false;
//     }
// }

bool operator==(const PipelineState& lhs, const PipelineState& rhs) {
    return memcmp(&lhs, &rhs, sizeof(PipelineState)) == 0;
}

PipelineCache::PipelineCache(const PipelineCacheCreateInfo& pipelineCacheCreateInfo)
    : m_renderer(pipelineCacheCreateInfo.renderer) {
}

duk::rhi::GraphicsPipeline* PipelineCache::find(const PipelineState& state) {
    if (const auto it = m_pipelines.find(state); it != m_pipelines.end()) {
        it->second.framesUnused = 0;
        return it->second.pipeline.get();
    }
    duk::rhi::RHI::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.viewport = state.viewport;
    pipelineCreateInfo.scissor = state.scissor;
    pipelineCreateInfo.cullModeMask = state.cullModeMask;
    pipelineCreateInfo.fillMode = state.fillMode;
    pipelineCreateInfo.topology = state.topology;
    pipelineCreateInfo.shader = state.shader;
    pipelineCreateInfo.renderPass = state.renderPass;
    pipelineCreateInfo.blend = state.blend;
    pipelineCreateInfo.depthTesting = state.depthTest;

    auto pipeline = m_renderer->rhi()->create_graphics_pipeline(pipelineCreateInfo);
    if (!pipeline) {
        throw std::runtime_error("failed to create Pipeline!");
    }
    PipelineEntry entry = {};
    entry.pipeline = std::move(pipeline);
    entry.framesUnused = 0;
    auto result = m_pipelines.emplace(state, entry);
    if (!result.second) {
        throw std::runtime_error("failed to insert Pipeline into cache");
    }
    return entry.pipeline.get();
}

void PipelineCache::clear() {
    const auto kDeleteUnusedPipelineAfterFrames = 100;

    for (auto it = m_pipelines.begin(); it != m_pipelines.end(); ++it) {
        auto& entry = it->second;
        entry.framesUnused++;

        if (entry.framesUnused > kDeleteUnusedPipelineAfterFrames) {
            it = m_pipelines.erase(it);
        }
    }
}
}// namespace duk::renderer
