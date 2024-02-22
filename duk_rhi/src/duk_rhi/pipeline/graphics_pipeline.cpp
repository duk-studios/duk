/// 18/04/2023
/// pipeline.cpp

#include <duk_rhi/pipeline/graphics_pipeline.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/render_pass.h>

namespace duk::rhi {

GraphicsPipeline::~GraphicsPipeline() = default;

void GraphicsPipeline::set_viewport(const glm::vec2& extent, const glm::vec2& offset, float minDepth, float maxDepth) {
    set_viewport({extent, offset, minDepth, maxDepth});
}

void GraphicsPipeline::set_scissor(const glm::ivec2& extent, const glm::u32vec2& offset) {
    set_scissor(Scissor{extent, offset});
}

void GraphicsPipeline::set_blend_src_color_factor(Blend::Factor srcColorBlendFactor) {
    auto blend = this->blend();
    blend.srcColorBlendFactor = srcColorBlendFactor;
    set_blend(blend);
}

void GraphicsPipeline::set_blend_dst_color_factor(Blend::Factor dstColorBlendFactor) {
    auto blend = this->blend();
    blend.dstColorBlendFactor = dstColorBlendFactor;
    set_blend(blend);
}

void GraphicsPipeline::set_blend_color_op(Blend::Operator colorBlendOp) {
    auto blend = this->blend();
    blend.colorBlendOp = colorBlendOp;
    set_blend(blend);
}

void GraphicsPipeline::set_blend_src_alpha_factor(Blend::Factor srcAlphaBlendFactor) {
    auto blend = this->blend();
    blend.srcAlphaBlendFactor = srcAlphaBlendFactor;
    set_blend(blend);
}

void GraphicsPipeline::set_blend_dst_alpha_factor(Blend::Factor dstAlphaBlendFactor) {
    auto blend = this->blend();
    blend.dstAlphaBlendFactor = dstAlphaBlendFactor;
    set_blend(blend);
}

void GraphicsPipeline::set_blend_alpha_op(Blend::Operator alphaBlendOp) {
    auto blend = this->blend();
    blend.alphaBlendOp = alphaBlendOp;
    set_blend(blend);
}

void GraphicsPipeline::set_blend_enabled(bool enabled) {
    auto blend = this->blend();
    blend.enabled = enabled;
    set_blend(blend);
}

duk::hash::Hash GraphicsPipeline::hash_of(GraphicsPipeline::Viewport viewport,
                                          GraphicsPipeline::Scissor scissor,
                                          GraphicsPipeline::Blend blend,
                                          Shader* shader,
                                          RenderPass* renderPass,
                                          GraphicsPipeline::CullMode::Mask cullMode,
                                          bool depthTesting) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, viewport.offset);
    duk::hash::hash_combine(hash, viewport.extent);
    duk::hash::hash_combine(hash, blend.alphaBlendOp);
    duk::hash::hash_combine(hash, blend.colorBlendOp);
    duk::hash::hash_combine(hash, blend.dstAlphaBlendFactor);
    duk::hash::hash_combine(hash, blend.srcAlphaBlendFactor);
    duk::hash::hash_combine(hash, blend.dstColorBlendFactor);
    duk::hash::hash_combine(hash, blend.srcColorBlendFactor);
    duk::hash::hash_combine(hash, blend.enabled);
    duk::hash::hash_combine(hash, scissor.extent);
    duk::hash::hash_combine(hash, scissor.offset);
    duk::hash::hash_combine(hash, shader->hash());
    duk::hash::hash_combine(hash, renderPass->hash());
    duk::hash::hash_combine(hash, cullMode);
    duk::hash::hash_combine(hash, depthTesting);
    return hash;
}

}// namespace duk::rhi