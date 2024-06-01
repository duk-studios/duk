//
// Created by Ricardo on 04/05/2024.
//

#include <duk_renderer/renderer.h>
#include <duk_renderer/shader/pipeline_cache.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

bool operator==(const PipelineState& lhs, const PipelineState& rhs) {
    return memcmp(&lhs, &rhs, sizeof(PipelineState)) == 0;
}

PipelineCache::PipelineCache(const PipelineCacheCreateInfo& pipelineCacheCreateInfo)
    : m_renderer(pipelineCacheCreateInfo.renderer) {
}

std::shared_ptr<duk::rhi::GraphicsPipeline> PipelineCache::find(const PipelineState& state) {
    if (const auto it = m_pipelines.find(state); it != m_pipelines.end()) {
        it->second.framesUnused = 0;
        return it->second.pipeline;
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
    return entry.pipeline;
}

void PipelineCache::clear() {
    m_pipelines.clear();
}

}// namespace duk::renderer
