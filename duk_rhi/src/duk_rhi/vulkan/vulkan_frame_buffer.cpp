/// 23/04/2023
/// vulkan_frame_buffer.cpp

#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>

#include <limits>
#include <span>
#include <stdexcept>

namespace duk::rhi {

VulkanFrameBuffer::VulkanFrameBuffer(const VulkanFrameBufferCreateInfo& vulkanFrameBufferCreateInfo)
    : m_device(vulkanFrameBufferCreateInfo.device)
    , m_renderPass(vulkanFrameBufferCreateInfo.renderPass)
    , m_attachments(vulkanFrameBufferCreateInfo.attachments, vulkanFrameBufferCreateInfo.attachments + vulkanFrameBufferCreateInfo.attachmentCount) {
    update_extent();

    std::vector<VkImageView> attachmentViews(m_attachments.size());
    for (size_t i = 0; i < m_attachments.size(); i++) {
        attachmentViews[i] = m_attachments[i]->image_view(vulkanFrameBufferCreateInfo.imageIndex);
    }

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass->handle();
    framebufferCreateInfo.width = m_width;
    framebufferCreateInfo.height = m_height;
    framebufferCreateInfo.layers = 1;
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentViews.size());
    framebufferCreateInfo.pAttachments = attachmentViews.data();

    if (vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_frameBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkFramebuffer");
    }
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
    clean();
}

void VulkanFrameBuffer::clean() {
    if (m_frameBuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_device, m_frameBuffer, nullptr);
        m_frameBuffer = VK_NULL_HANDLE;
    }
}

VkFramebuffer VulkanFrameBuffer::handle() const {
    return m_frameBuffer;
}

uint32_t VulkanFrameBuffer::width() const {
    return m_width;
}

uint32_t VulkanFrameBuffer::height() const {
    return m_height;
}

glm::uvec2 VulkanFrameBuffer::size() const {
    return {m_width, m_height};
}

Image* VulkanFrameBuffer::at(uint32_t attachment) const {
    return m_attachments.at(attachment);
}

void VulkanFrameBuffer::update_extent() {
    m_width = std::numeric_limits<uint32_t>::max();
    m_height = std::numeric_limits<uint32_t>::max();
    for (auto& attachment: m_attachments) {
        m_width = std::min(m_width, attachment->width());
        m_height = std::min(m_height, attachment->height());
    }
}

}// namespace duk::rhi

