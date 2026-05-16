/// vulkan_single_set_resource_state.h

#ifndef DUK_RHI_VULKAN_SINGLE_SET_RESOURCE_STATE_H
#define DUK_RHI_VULKAN_SINGLE_SET_RESOURCE_STATE_H

#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set_allocator.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_resource_state.h>

#include <memory>

namespace duk::rhi {

class VulkanSamplerCache;

struct VulkanSingleSetResourceBinderCreateInfo {
    VkDevice device;
    VulkanSamplerCache* samplerCache;
    const VulkanPhysicalDevice* physicalDevice;
    uint32_t maxDescriptorSets{1024};
};

/// Allocates or reuses one VkDescriptorSet (set=0) per unique resource combination.
/// Descriptor sets are cached by a hash of the bound resource handles and reused
/// across frames when the same resources are bound again.
class VulkanSingleSetResourceState : public VulkanResourceState {
public:
    explicit VulkanSingleSetResourceState(const VulkanSingleSetResourceBinderCreateInfo& createInfo);

    DUK_NO_DISCARD VkPipelineLayout pipeline_layout(const ShaderBindingLayout& bindingLayout) override;

    void bind_resources(VkCommandBuffer commandBuffer,
                        VkPipelineBindPoint bindPoint,
                        const VulkanShader& shader,
                        const ShaderResources& resources,
                        uint32_t frameIndex) override;

private:
    VkDevice m_device;
    VulkanSamplerCache* m_samplerCache;
    VkDeviceSize m_uniformBufferOffsetAlignment;
    VkDeviceSize m_storageBufferOffsetAlignment;
    std::unique_ptr<VulkanDescriptorSetLayoutCache> m_descriptorSetLayoutCache;
    std::unique_ptr<VulkanDescriptorSetCache> m_descriptorSetCache;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_SINGLE_SET_RESOURCE_STATE_H

