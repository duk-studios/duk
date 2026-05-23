/// vulkan_resource_state.h

#ifndef DUK_RHI_VULKAN_RESOURCE_STATE_H
#define DUK_RHI_VULKAN_RESOURCE_STATE_H

#include <duk_rhi/command_context.h>
#include <duk_rhi/shader.h>
#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

class VulkanShader;

/// Abstract interface responsible for binding shader resources to the command buffer.
/// Concrete implementations decide the descriptor strategy (e.g. single set, descriptor heaps).
class VulkanResourceState {
public:
    virtual ~VulkanResourceState() = default;

    DUK_NO_DISCARD virtual VkPipelineLayout pipeline_layout(const ShaderBindingLayout& bindingLayout) = 0;

    virtual void bind_resources(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, const VulkanShader& shader, const ShaderResources& resources, uint32_t frameIndex) = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_RESOURCE_STATE_H
