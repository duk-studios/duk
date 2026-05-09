/// 23/04/2023
/// vulkan_frame_buffer.h

#ifndef DUK_RHI_VULKAN_FRAME_BUFFER_H
#define DUK_RHI_VULKAN_FRAME_BUFFER_H

#include <duk_rhi/frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <vector>

namespace duk::rhi {

class VulkanRenderPass;

struct VulkanFrameBufferCreateInfo {
    VkDevice device;
    VulkanRenderPass* renderPass;
    VulkanImage** attachments;
    uint32_t attachmentCount;
    /// imageIndex passed when sampling image views from VulkanImage attachments.
    /// For VulkanMemoryImage this is ignored; use the swapchain image index here
    /// when building a framebuffer for a specific swapchain image.
    uint32_t imageIndex;
};

class VulkanFrameBuffer : public FrameBuffer {
public:
    explicit VulkanFrameBuffer(const VulkanFrameBufferCreateInfo& vulkanFrameBufferCreateInfo);

    ~VulkanFrameBuffer() override;

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD glm::uvec2 size() const override;

    DUK_NO_DISCARD Image* at(uint32_t attachment) const override;

    void clean();

    DUK_NO_DISCARD VkFramebuffer handle() const;

private:
    void update_extent();

private:
    uint32_t m_width;
    uint32_t m_height;
    VkDevice m_device;
    VulkanRenderPass* m_renderPass;
    std::vector<VulkanImage*> m_attachments;
    VkFramebuffer m_frameBuffer{VK_NULL_HANDLE};
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_FRAME_BUFFER_H
