//
// Created by Ricardo on 03/05/2024.
//
#include <duk_renderer/renderer.h>
#include <duk_renderer/shader/shader_pipeline.h>
#include <duk_rhi/pipeline/std_shader_data_source.h>

namespace duk::renderer {

ShaderPipeline::ShaderPipeline(const ShaderPipelineCreateInfo& shaderPipelineCreateInfo)
    : m_renderer(shaderPipelineCreateInfo.renderer) {
    const auto pipelineData = shaderPipelineCreateInfo.shaderPipelineData;

    m_shaderModules[duk::rhi::ShaderModule::VERTEX] = pipelineData->shaders.vert;
    m_shaderModules[duk::rhi::ShaderModule::FRAGMENT] = pipelineData->shaders.frag;

    const auto& settings = pipelineData->settings;
    set_blend(settings.blend);
    set_depth(settings.depth);
    set_invert_y(settings.invertY);
    m_priority = settings.priority;

    m_state.cullModeMask = settings.cullModeMask;
    m_state.fillMode = duk::rhi::GraphicsPipeline::FillMode::FILL;
    m_state.topology = duk::rhi::GraphicsPipeline::Topology::TRIANGLE_LIST;
    create_shader();
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
    } else {
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

uint32_t ShaderPipeline::priority() const {
    return m_priority;
}

void ShaderPipeline::update(PipelineCache& pipelineCache, duk::rhi::RenderPass* renderPass, const glm::vec2& viewport) {
    bool dirty = m_dirty;
    if (m_state.renderPass != renderPass) {
        m_state.renderPass = renderPass;
        dirty = true;
    }
    if (m_state.viewport.extent != viewport || dirty) {//check for dirty, invert y might have changed
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
    commandBuffer->bind_graphics_pipeline(m_pipeline.get());
}

void ShaderPipeline::solve_resources(duk::resource::DependencySolver* solver) {
    for (auto& [type, module]: m_shaderModules) {
        solver->solve(module);
    }
}

void ShaderPipeline::solve_resources(duk::resource::ReferenceSolver* solver) {
    for (auto& [type, module]: m_shaderModules) {
        solver->solve(module);
    }
    create_shader();
}

void ShaderPipeline::create_shader() {
    if (m_shader) {
        return;
    }
    if (!m_shaderModules[duk::rhi::ShaderModule::VERTEX] || !m_shaderModules[duk::rhi::ShaderModule::FRAGMENT]) {
        return;
    }
    const auto rhi = m_renderer->rhi();

    duk::rhi::StdShaderDataSource shaderDataSource = {};
    {
        auto& module = m_shaderModules[duk::rhi::ShaderModule::VERTEX];
        shaderDataSource.insert_spir_v_code(duk::rhi::ShaderModule::VERTEX, module->data(), module->size());
    }
    {
        auto& module = m_shaderModules[duk::rhi::ShaderModule::FRAGMENT];
        shaderDataSource.insert_spir_v_code(duk::rhi::ShaderModule::FRAGMENT, module->data(), module->size());
    }
    shaderDataSource.update_hash();

    duk::rhi::RHI::ShaderCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.shaderDataSource = &shaderDataSource;
    m_shader = rhi->create_shader(shaderCreateInfo);
}

}// namespace duk::renderer
