//
// Created by Ricardo on 04/05/2024.
//

#ifndef DUK_RENDERER_PIPELINE_CACHE_H
#define DUK_RENDERER_PIPELINE_CACHE_H

#include <duk_rhi/pipeline/graphics_pipeline.h>

#include <memory>

namespace duk::renderer {

struct PipelineState {
    duk::rhi::Shader* shader;
    duk::rhi::RenderPass* renderPass;
    duk::rhi::GraphicsPipeline::Viewport viewport;
    duk::rhi::GraphicsPipeline::Scissor scissor;
    duk::rhi::GraphicsPipeline::Blend blend;
    duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask;
    duk::rhi::GraphicsPipeline::Topology topology;
    duk::rhi::GraphicsPipeline::FillMode fillMode;
    bool depthTest;
};

bool operator==(const PipelineState& lhs, const PipelineState& rhs);

}

template<>
struct std::hash<duk::renderer::PipelineState> {
    size_t operator()(const duk::renderer::PipelineState& state) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, state.shader);
        duk::hash::hash_combine(hash, state.viewport);
        duk::hash::hash_combine(hash, state.scissor);
        duk::hash::hash_combine(hash, state.blend);
        duk::hash::hash_combine(hash, state.cullModeMask);
        duk::hash::hash_combine(hash, state.topology);
        duk::hash::hash_combine(hash, state.fillMode);
        return hash;
    }
};

namespace duk::renderer {

class Renderer;

struct PipelineCacheCreateInfo {
    Renderer* renderer;
};

class PipelineCache {
public:
    PipelineCache(const PipelineCacheCreateInfo& pipelineCacheCreateInfo);

    duk::rhi::GraphicsPipeline* find(const PipelineState& state);

    void clear();

private:
    struct PipelineEntry {
        std::shared_ptr<duk::rhi::GraphicsPipeline> pipeline;
        int framesUnused;
    };

private:
    Renderer* m_renderer;
    std::unordered_map<PipelineState, PipelineEntry> m_pipelines;
};

}

#endif //DUK_RENDERER_PIPELINE_CACHE_H
