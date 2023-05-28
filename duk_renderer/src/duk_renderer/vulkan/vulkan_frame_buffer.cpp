/// 23/04/2023
/// vulkan_frame_buffer.cpp

#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_swapchain.h>
#include <duk_renderer/vulkan/vulkan_render_pass.h>

#include <span>

namespace duk::renderer {

VulkanFrameBuffer::VulkanFrameBuffer(const VulkanFrameBufferCreateInfo& vulkanFrameBufferCreateInfo) :
    m_device(vulkanFrameBufferCreateInfo.device),
    m_renderPass(vulkanFrameBufferCreateInfo.renderPass),
    m_attachments(vulkanFrameBufferCreateInfo.attachments),
    m_attachmentCount(vulkanFrameBufferCreateInfo.attachmentCount) {
    create(vulkanFrameBufferCreateInfo.imageCount);
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
    clean();
}

void VulkanFrameBuffer::create(uint32_t imageCount) {

    m_width = std::numeric_limits<uint32_t>::max();
    m_height = std::numeric_limits<uint32_t>::max();
    for (auto it = m_attachments, endAttachment = m_attachments + m_attachmentCount; it != endAttachment; it++) {
        m_width = std::min(m_width, it->width());
        m_height = std::min(m_height, it->height());
    }

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass->handle();
    framebufferCreateInfo.width = m_width;
    framebufferCreateInfo.height = m_height;
    framebufferCreateInfo.layers = 1;

    m_frameBuffers.resize(imageCount);
    for (int i = 0; i < m_frameBuffers.size(); i++) {

        std::vector<VkImageView> attachments(m_attachmentCount);
        for (int j = 0; j < m_attachmentCount; j++) {
            attachments[j] = m_attachments[j].image_view(i);
        }

        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();

        vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_frameBuffers[i]);
    }
}

void VulkanFrameBuffer::clean() {
    for (auto& frameBuffer : m_frameBuffers) {
        vkDestroyFramebuffer(m_device, frameBuffer, nullptr);
    }
    m_frameBuffers.clear();
}

VkFramebuffer VulkanFrameBuffer::handle(uint32_t frameIndex) const {
    return m_frameBuffers[frameIndex];
}

uint32_t VulkanFrameBuffer::width() const {
    return m_width;
}

uint32_t VulkanFrameBuffer::height() const {
    return m_height;
}

}