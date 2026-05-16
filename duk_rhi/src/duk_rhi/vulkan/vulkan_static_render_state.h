/// static_render_state_vk.h

#ifndef DUK_RHI_VULKAN_STATIC_RENDER_STATE_VK_H
#define DUK_RHI_VULKAN_STATIC_RENDER_STATE_VK_H

#include <duk_rhi/vulkan/vulkan_render_state.h>
#include <duk_rhi/vulkan/vulkan_deletion_queue.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <cstdint>
#include <unordered_map>

namespace duk::rhi {

class VulkanShader;

struct VulkanStaticRenderStateCreateInfo {
    VkDevice device;
    VulkanDeletionQueue* deletionQueue;
};

/// Concrete VulkanRenderState that uses legacy Vulkan objects:
/// - VkRenderPass (keyed and cached by attachment format/aspect),
/// - VkFramebuffer (keyed and cached by render pass + image views),
/// - VkPipeline (keyed and cached by PipelineState + shader hash + render pass).
///
/// Lazily begins a new render pass when framebuffer compatibility changes,
/// and lazily binds a pipeline when PipelineState or shader changes.
///
/// All cached objects are destroyed directly in the destructor (GPU-idle assumed
/// at shutdown). The deletionQueue is available for runtime eviction needs.
class VulkanStaticRenderState : public VulkanRenderState {
public:
    explicit VulkanStaticRenderState(const VulkanStaticRenderStateCreateInfo& createInfo);

    ~VulkanStaticRenderState() override;

    void begin(VkCommandBuffer commandBuffer,
               const VulkanFrameBuffer& frameBuffer,
               VkAttachmentLoadOp loadOp,
               VkAttachmentStoreOp storeOp,
               const glm::vec4& clearColor) override;

    void bind_pipeline(VkCommandBuffer commandBuffer,
                       const VulkanShader& shader,
                       const PipelineState& state,
                       VkPipelineLayout pipelineLayout) override;

    void end(VkCommandBuffer commandBuffer) override;

private:
    struct CachedRenderPass {
        VkRenderPass handle{VK_NULL_HANDLE};
        uint32_t colorAttachmentCount{0};
    };

    CachedRenderPass get_or_create_render_pass(const VulkanFrameBuffer& frameBuffer,
                                               VkAttachmentLoadOp loadOp,
                                               VkAttachmentStoreOp storeOp);

    VkFramebuffer get_or_create_framebuffer(VkRenderPass renderPass,
                                            const VulkanFrameBuffer& frameBuffer);

    VkPipeline get_or_create_pipeline(VkRenderPass renderPass,
                                      uint32_t colorAttachmentCount,
                                      const VulkanShader& shader,
                                      const PipelineState& state,
                                      VkPipelineLayout pipelineLayout);

private:
    VkDevice m_device;
    VulkanDeletionQueue* m_deletionQueue;

    std::unordered_map<size_t, CachedRenderPass> m_renderPassCache;
    std::unordered_map<size_t, VkFramebuffer> m_framebufferCache;
    std::unordered_map<size_t, VkPipeline> m_pipelineCache;

    // Active render pass state
    VkRenderPass m_activeRenderPass{VK_NULL_HANDLE};
    VkFramebuffer m_activeFramebuffer{VK_NULL_HANDLE};
    VkPipeline m_activePipeline{VK_NULL_HANDLE};
    uint32_t m_activeColorAttachmentCount{0};
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_STATIC_RENDER_STATE_VK_H

