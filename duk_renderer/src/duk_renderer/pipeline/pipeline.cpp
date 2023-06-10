/// 18/04/2023
/// pipeline.cpp

#include <duk_renderer/pipeline/pipeline.h>

namespace duk::renderer {

Pipeline::~Pipeline() = default;

void Pipeline::set_viewport(const glm::vec2& extent, const glm::vec2& offset, float minDepth, float maxDepth) {
    set_viewport({extent, offset, minDepth, maxDepth});
}

void Pipeline::set_scissor(const glm::ivec2& extent, const glm::u32vec2& offset) {
    set_scissor(Scissor{extent, offset});
}

void Pipeline::set_blend_src_color_factor(Blend::Factor srcColorBlendFactor) {
    auto blend = this->blend();
    blend.srcColorBlendFactor = srcColorBlendFactor;
    set_blend(blend);
}

void Pipeline::set_blend_dst_color_factor(Blend::Factor dstColorBlendFactor) {
    auto blend = this->blend();
    blend.dstColorBlendFactor = dstColorBlendFactor;
    set_blend(blend);
}

void Pipeline::set_blend_color_op(Blend::Operator colorBlendOp) {
    auto blend = this->blend();
    blend.colorBlendOp = colorBlendOp;
    set_blend(blend);
}

void Pipeline::set_blend_src_alpha_factor(Blend::Factor srcAlphaBlendFactor) {
    auto blend = this->blend();
    blend.srcAlphaBlendFactor = srcAlphaBlendFactor;
    set_blend(blend);
}

void Pipeline::set_blend_dst_alpha_factor(Blend::Factor dstAlphaBlendFactor) {
    auto blend = this->blend();
    blend.dstAlphaBlendFactor = dstAlphaBlendFactor;
    set_blend(blend);
}

void Pipeline::set_blend_alpha_op(Blend::Operator alphaBlendOp) {
    auto blend = this->blend();
    blend.alphaBlendOp = alphaBlendOp;
    set_blend(blend);
}

void Pipeline::set_blend_enabled(bool enabled) {
    auto blend = this->blend();
    blend.enabled = enabled;
    set_blend(blend);
}

}