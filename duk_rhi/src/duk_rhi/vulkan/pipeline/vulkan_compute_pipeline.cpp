/// 13/06/2023
/// vulkan_compute_pipeline.cpp

#include <duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h>

#include <stdexcept>

namespace duk::rhi {

VulkanComputePipeline::VulkanComputePipeline(const VulkanComputePipelineCreateInfo& computePipelineCreateInfo)
    : m_device(computePipelineCreateInfo.device)
    , m_shader(computePipelineCreateInfo.shader) {
    VkPipelineShaderStageCreateInfo computeShaderStage = {};
    computeShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStage.module = m_shader->shader_modules().at(ShaderModule::COMPUTE);
    computeShaderStage.pName = "main";

    VkComputePipelineCreateInfo computePipeline = {};
    computePipeline.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipeline.layout = m_shader->pipeline_layout();
    computePipeline.stage = computeShaderStage;

    if (vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipeline, nullptr, &m_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkPipeline (compute)");
    }
}

VulkanComputePipeline::~VulkanComputePipeline() {
    destroy();
}

void VulkanComputePipeline::destroy() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }
}

VkPipeline VulkanComputePipeline::handle() const {
    return m_pipeline;
}

VkPipelineLayout VulkanComputePipeline::pipeline_layout() const {
    return m_shader->pipeline_layout();
}

}// namespace duk::rhi