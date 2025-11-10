/// 23/04/2023
/// vulkan_frame_buffer.h

#ifndef DUK_RHI_VULKAN_FRAME_BUFFER_H
#define DUK_RHI_VULKAN_FRAME_BUFFER_H

#include <duk_rhi/frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <vector>

namespace duk::rhi {

class VulkanSwapchain;
class VulkanRenderPass;
class VulkanResourceManager;

struct VulkanFrameBufferCreateInfo {
    uint32_t imageCount;
    VkDevice device;
    VulkanRenderPass* renderPass;
    VulkanImage** attachments;
    uint32_t attachmentCount;
    VulkanResourceManager* resourceManager;
};

class VulkanFrameBuffer : public FrameBuffer {
public:
    explicit VulkanFrameBuffer(const VulkanFrameBufferCreateInfo& vulkanFrameBufferCreateInfo);

    ~VulkanFrameBuffer() override;

    void update(uint32_t imageIndex);

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD glm::uvec2 size() const override;

    DUK_NO_DISCARD Image* at(uint32_t attachment) const override;

    void create(uint32_t imageCount);

    void clean();

    void clean(uint32_t imageIndex);

    DUK_NO_DISCARD VkFramebuffer handle(uint32_t frameIndex) const;

private:
    void update_extent();

private:
    uint32_t m_width;
    uint32_t m_height;
    VkDevice m_device;
    VulkanRenderPass* m_renderPass;
    VulkanResourceManager* m_resourceManager;
    std::vector<VulkanImage*> m_attachments;
    std::vector<VkFramebuffer> m_frameBuffers;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_FRAME_BUFFER_H
