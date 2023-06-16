/// 13/06/2023
/// vulkan_compute_pipeline.h

#ifndef DUK_RENDERER_VULKAN_COMPUTE_PIPELINE_H
#define DUK_RENDERER_VULKAN_COMPUTE_PIPELINE_H

#include <duk_renderer/pipeline/compute_pipeline.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/pipeline/vulkan_shader.h>

namespace duk::renderer {

struct VulkanComputePipelineCreateInfo {
    VkDevice device;
    VulkanShader* shader;
};


class VulkanComputePipeline : public ComputePipeline {
public:

    explicit VulkanComputePipeline(const VulkanComputePipelineCreateInfo& computePipelineCreateInfo);

    ~VulkanComputePipeline() override;

    void create(uint32_t imageCount);

    void clean(uint32_t imageIndex);

    void clean();

    void update(uint32_t imageIndex);

    DUK_NO_DISCARD VkPipeline handle(uint32_t imageIndex) const;

    DUK_NO_DISCARD VkPipelineLayout pipeline_layout() const;

private:
    VkDevice m_device;
    VulkanShader* m_shader;
    VkPipeline m_pipeline;
    duk::hash::Hash m_hash;
    duk::hash::Hash m_pipelineHash;
};


}

#endif // DUK_RENDERER_VULKAN_COMPUTE_PIPELINE_H

