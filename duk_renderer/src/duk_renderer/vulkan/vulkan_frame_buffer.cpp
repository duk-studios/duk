/// 23/04/2023
/// vulkan_frame_buffer.cpp

#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_swapchain.h>
#include <duk_renderer/vulkan/vulkan_render_pass.h>

namespace duk::renderer {

VulkanFrameBuffer::VulkanFrameBuffer(const VulkanFrameBufferCreateInfo& vulkanFrameBufferCreateInfo) :
    m_width(vulkanFrameBufferCreateInfo.width),
    m_height(vulkanFrameBufferCreateInfo.height),
    m_frameCount(vulkanFrameBufferCreateInfo.frameCount),
    m_device(vulkanFrameBufferCreateInfo.device),
    m_renderPass(vulkanFrameBufferCreateInfo.renderPass),
    m_attachments(vulkanFrameBufferCreateInfo.attachments),
    m_attachmentCount(vulkanFrameBufferCreateInfo.attachmentCount) {
    create();
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
    clean();
}

void VulkanFrameBuffer::create() {
    clean();

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass->handle();
    framebufferCreateInfo.width = m_width;
    framebufferCreateInfo.height = m_height;
    framebufferCreateInfo.layers = 1;

    m_frameBuffers.resize(m_frameCount);
    for (int i = 0; i < m_frameBuffers.size(); i++) {

        std::vector<VkImageView> attachments(m_attachmentCount);
        for (int j = 0; j < m_attachmentCount; j++){
            attachments[j] = m_attachments[j].image_view(i);
        }

        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();

        vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_frameBuffers[i]);
    }
}

void VulkanFrameBuffer::clean() {
    for (auto frameBuffer : m_frameBuffers) {
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