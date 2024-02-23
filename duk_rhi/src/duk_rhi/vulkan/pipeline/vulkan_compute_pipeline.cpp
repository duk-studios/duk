/// 13/06/2023
/// vulkan_compute_pipeline.cpp

#include <duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h>

namespace duk::rhi {

VulkanComputePipeline::VulkanComputePipeline(const VulkanComputePipelineCreateInfo& computePipelineCreateInfo)
    : m_device(computePipelineCreateInfo.device)
    , m_shader(computePipelineCreateInfo.shader)
    , m_pipeline(VK_NULL_HANDLE)
    , m_hash(m_shader->hash())
    , m_pipelineHash(duk::hash::kUndefinedHash) {
}

VulkanComputePipeline::~VulkanComputePipeline() {
    clean();
}

void VulkanComputePipeline::create(uint32_t) {
    // do nothing, as compute pipelines are not created per swapchain image
}

void VulkanComputePipeline::clean(uint32_t) {
    // do nothing, same as create
}

void VulkanComputePipeline::clean() {
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
}

void VulkanComputePipeline::update(uint32_t) {
    if (m_pipelineHash == m_hash) {
        return;
    }
    m_pipelineHash = m_hash;

    VkPipelineShaderStageCreateInfo computeShaderStage = {};
    computeShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStage.module = m_shader->shader_modules().at(Shader::Module::COMPUTE);
    computeShaderStage.pName = "main";

    VkComputePipelineCreateInfo computePipeline = {};
    computePipeline.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipeline.layout = m_shader->pipeline_layout();
    computePipeline.stage = computeShaderStage;

    vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipeline, nullptr, &m_pipeline);
}

VkPipeline VulkanComputePipeline::handle(uint32_t) const {
    return m_pipeline;
}

VkPipelineLayout VulkanComputePipeline::pipeline_layout() const {
    return m_shader->pipeline_layout();
}

}// namespace duk::rhi