/// 13/06/2023
/// vulkan_compute_pipeline.h

#ifndef DUK_RHI_VULKAN_COMPUTE_PIPELINE_H
#define DUK_RHI_VULKAN_COMPUTE_PIPELINE_H

#include <duk_rhi/pipeline/compute_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

struct VulkanComputePipelineCreateInfo {
    VkDevice device;
    VulkanShader* shader;
};

class VulkanComputePipeline : public ComputePipeline {
public:
    explicit VulkanComputePipeline(const VulkanComputePipelineCreateInfo& computePipelineCreateInfo);

    ~VulkanComputePipeline() override;

    DUK_NO_DISCARD VkPipeline handle() const;

    DUK_NO_DISCARD VkPipelineLayout pipeline_layout() const;

private:
    void destroy();

private:
    VkDevice m_device;
    VulkanShader* m_shader;
    VkPipeline m_pipeline{VK_NULL_HANDLE};
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_COMPUTE_PIPELINE_H
