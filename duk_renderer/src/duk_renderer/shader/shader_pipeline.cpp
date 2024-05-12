//
// Created by Ricardo on 03/05/2024.
//
#include <duk_renderer/renderer.h>
#include <duk_renderer/shader/shader_pipeline.h>

namespace duk::renderer {

ShaderPipeline::ShaderPipeline(const ShaderPipelineCreateInfo& shaderPipelineCreateInfo)
    : m_renderer(shaderPipelineCreateInfo.renderer) {
    {
        const auto rhi = m_renderer->rhi();
        duk::rhi::RHI::ShaderCreateInfo shaderCreateInfo = {};
        shaderCreateInfo.shaderDataSource = shaderPipelineCreateInfo.shaderDataSource;
        m_shader = rhi->create_shader(shaderCreateInfo);
    }

    const auto& settings = shaderPipelineCreateInfo.settings;
    set_blend(settings.blend);
    set_depth(settings.depth);
    set_invert_y(settings.invertY);

    m_state.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
    m_state.fillMode = duk::rhi::GraphicsPipeline::FillMode::FILL;
    m_state.topology = duk::rhi::GraphicsPipeline::Topology::TRIANGLE_LIST;
}

const duk::rhi::DescriptorSetDescription& ShaderPipeline::descriptor_set_description() const {
    return m_shader->descriptor_set_description();
}

void ShaderPipeline::set_blend(bool enable) {
    if (enable) {
        m_state.blend.enabled = true;
        m_state.blend.colorBlendOp = rhi::GraphicsPipeline::Blend::Operator::ADD;
        m_state.blend.srcColorBlendFactor = rhi::GraphicsPipeline::Blend::Factor::SRC_ALPHA;
        m_state.blend.dstColorBlendFactor = rhi::GraphicsPipeline::Blend::Factor::ONE_MINUS_SRC_ALPHA;
        m_state.blend.alphaBlendOp = rhi::GraphicsPipeline::Blend::Operator::ADD;
        m_state.blend.srcAlphaBlendFactor = rhi::GraphicsPipeline::Blend::Factor::ONE;
        m_state.blend.dstAlphaBlendFactor = rhi::GraphicsPipeline::Blend::Factor::ZERO;
    }
    else {
        m_state.blend.enabled = false;
    }
    m_dirty = true;
}

bool ShaderPipeline::blend() const {
    return m_state.blend.enabled;
}

void ShaderPipeline::set_depth(bool enable) {
    m_state.depthTest = enable;
    m_dirty = true;
}

bool ShaderPipeline::depth() const {
    return m_state.depthTest;
}

void ShaderPipeline::set_invert_y(bool invert) {
    m_invertY = invert;
    m_dirty = true;
}

bool ShaderPipeline::invert_y() const {
    return m_invertY;
}

void ShaderPipeline::update(PipelineCache& pipelineCache, duk::rhi::RenderPass* renderPass, const glm::vec2& viewport) {
    bool dirty = m_dirty;
    if (m_state.renderPass != renderPass) {
        m_state.renderPass = renderPass;
        dirty = true;
    }
    if (m_state.viewport.extent != viewport || dirty) { //check for dirty, invert y might have changed
        m_state.viewport.extent = viewport;
        m_state.viewport.offset = {};
        m_state.viewport.maxDepth = 1.0f;
        m_state.viewport.minDepth = 0.0f;
        if (m_invertY) {
            m_state.viewport.offset.y = m_state.viewport.extent.y;
            m_state.viewport.extent.y = -m_state.viewport.extent.y;
        }

        m_state.scissor.extent = viewport;
        m_state.scissor.offset = {};

        dirty = true;
    }
    if (m_state.shader != m_shader.get()) {
        m_state.shader = m_shader.get();
        dirty = true;
    }

    if (dirty) {
        m_pipeline = pipelineCache.find(m_state);
        m_dirty = false;
    }
}

void ShaderPipeline::bind(duk::rhi::CommandBuffer* commandBuffer) const {
    commandBuffer->bind_graphics_pipeline(m_pipeline);
}

}
