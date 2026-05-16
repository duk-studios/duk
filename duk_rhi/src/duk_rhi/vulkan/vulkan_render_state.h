/// render_state_vk.h

#ifndef DUK_RHI_VULKAN_RENDER_STATE_VK_H
#define DUK_RHI_VULKAN_RENDER_STATE_VK_H

#include <duk_rhi/pipeline_state.h>
#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

class VulkanShader;
class VulkanFrameBuffer;

/// Abstract interface responsible for managing the GPU render state: render passes,
/// framebuffers, and graphics pipelines.
class VulkanRenderState {
public:
    virtual ~VulkanRenderState() = default;

    virtual void begin(VkCommandBuffer commandBuffer,
                       const VulkanFrameBuffer& frameBuffer,
                       VkAttachmentLoadOp loadOp,
                       VkAttachmentStoreOp storeOp,
                       const glm::vec4& clearColor) = 0;

    virtual void bind_pipeline(VkCommandBuffer commandBuffer,
                               const VulkanShader& shader,
                               const PipelineState& state,
                               VkPipelineLayout pipelineLayout) = 0;

    virtual void end(VkCommandBuffer commandBuffer) = 0;
};

VkAttachmentStoreOp convert_store_op(StoreOp storeOp);

VkAttachmentLoadOp convert_load_op(LoadOp loadOp);

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_RENDER_STATE_VK_H

