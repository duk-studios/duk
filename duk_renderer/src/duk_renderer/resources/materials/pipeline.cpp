/// 08/09/2023
/// pipeline.cpp

#include <duk_renderer/resources/materials/pipeline.h>
#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/brushes/brush.h>
#include <duk_renderer/renderer.h>

namespace duk::renderer {

static constexpr int kDeleteUnusedPipelineAfterFrames = 512;

Pipeline::Pipeline(const PipelineCreateInfo& pipelineCreateInfo) :
    m_renderer(pipelineCreateInfo.renderer),
    m_pipelineCache(m_renderer, pipelineCreateInfo.shaderDataSource) {

    m_listener.listen(m_renderer->render_start_event(), [this]{
        m_pipelineCache.clear_unused_pipelines();
    });
}

void Pipeline::use(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params) {
    commandBuffer->bind_graphics_pipeline(m_pipelineCache.find_pipeline_for_params(params));
}

void Pipeline::update_shader(duk::rhi::ShaderDataSource* shaderDataSource) {
    m_pipelineCache.update_shader(shaderDataSource);
}

void Pipeline::invert_y(bool invert) {
    m_pipelineCache.invert_y(invert);
}

void Pipeline::cull_mode_mask(duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask) {
    m_pipelineCache.cull_mode_mask(cullModeMask);
}

void Pipeline::clear_unused_pipelines() {
    m_pipelineCache.clear_unused_pipelines();
}

//------------------------------------

Pipeline::PipelineCache::PipelineCache(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource) :
    m_renderer(renderer),
    m_shader(nullptr),
    m_invertY(false),
    m_cullModeMask(duk::rhi::GraphicsPipeline::CullMode::BACK) {
    update_shader(shaderDataSource);
}

void Pipeline::PipelineCache::update_shader(const duk::rhi::ShaderDataSource* shaderDataSource) {
    duk::rhi::RHI::ShaderCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.shaderDataSource = shaderDataSource;

    m_shader = m_renderer->rhi()->create_shader(shaderCreateInfo);
}

duk::rhi::GraphicsPipeline* Pipeline::PipelineCache::find_pipeline_for_params(const DrawParams& params) {
    const auto hash = hash_for_params(params);

    auto it = m_pipelines.find(hash);
    if (it == m_pipelines.end()) {

        duk::rhi::RHI::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.viewport = viewport_for_params(params);
        pipelineCreateInfo.scissor.extent.x = params.outputWidth;
        pipelineCreateInfo.scissor.extent.y = params.outputHeight;
        pipelineCreateInfo.cullModeMask = m_cullModeMask;
        pipelineCreateInfo.fillMode = duk::rhi::GraphicsPipeline::FillMode::FILL;
        pipelineCreateInfo.topology = duk::rhi::GraphicsPipeline::Topology::TRIANGLE_LIST;
        pipelineCreateInfo.shader = m_shader.get();
        pipelineCreateInfo.renderPass = params.renderPass;
        pipelineCreateInfo.depthTesting = true;

        auto expectedPipeline = m_renderer->rhi()->create_graphics_pipeline(pipelineCreateInfo);
        if (!expectedPipeline) {
            throw std::runtime_error("failed to create Pipeline!");
        }

        PipelineEntry entry = {};
        entry.pipeline = expectedPipeline;
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

duk::hash::Hash Pipeline::PipelineCache::hash_for_params(const DrawParams& params) const {
    hash::Hash hash = 0;
    hash::hash_combine(hash, params.renderPass);
    hash::hash_combine(hash, params.outputWidth);
    hash::hash_combine(hash, params.outputHeight);
    hash::hash_combine(hash, m_invertY);
    hash::hash_combine(hash, m_cullModeMask);
    hash::hash_combine(hash, m_shader->hash());
    return hash;
}

duk::rhi::GraphicsPipeline::Viewport Pipeline::PipelineCache::viewport_for_params(const DrawParams& params) {
    duk::rhi::GraphicsPipeline::Viewport viewport = {};
    viewport.extent = {params.outputWidth, params.outputHeight};
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;
    if (m_invertY) {
        viewport.offset.y = viewport.extent.y;
        viewport.extent.y = -viewport.extent.y;
    }
    return viewport;
}

void Pipeline::PipelineCache::invert_y(bool invert) {
    m_invertY = invert;
}

void Pipeline::PipelineCache::cull_mode_mask(duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask) {
    m_cullModeMask = cullModeMask;
}

void Pipeline::PipelineCache::clear_unused_pipelines() {
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

}