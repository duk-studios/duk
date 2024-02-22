/// 23/04/2023
/// vulkan_frame_buffer.cpp

#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>

#include <span>

namespace duk::rhi {

VulkanFrameBuffer::VulkanFrameBuffer(const VulkanFrameBufferCreateInfo& vulkanFrameBufferCreateInfo) : m_device(vulkanFrameBufferCreateInfo.device),
                                                                                                       m_renderPass(vulkanFrameBufferCreateInfo.renderPass),
                                                                                                       m_attachments(vulkanFrameBufferCreateInfo.attachments, vulkanFrameBufferCreateInfo.attachments + vulkanFrameBufferCreateInfo.attachmentCount) {
    create(vulkanFrameBufferCreateInfo.imageCount);
    update_extent();
    update_hash();
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
    clean();
}

void VulkanFrameBuffer::update(uint32_t imageIndex) {
    auto& hash = m_frameBufferHashes[imageIndex];
    if (hash == m_hash) {
        return;
    }
    hash = m_hash;

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass->handle();
    framebufferCreateInfo.width = m_width;
    framebufferCreateInfo.height = m_height;
    framebufferCreateInfo.layers = 1;

    std::vector<VkImageView> attachments(m_attachments.size());
    for (int attachmentIndex = 0; attachmentIndex < attachments.size(); attachmentIndex++) {
        auto attachment = m_attachments[attachmentIndex];

        attachments[attachmentIndex] = attachment->image_view(imageIndex);
    }

    framebufferCreateInfo.attachmentCount = attachments.size();
    framebufferCreateInfo.pAttachments = attachments.data();

    vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_frameBuffers[imageIndex]);
}

void VulkanFrameBuffer::create(uint32_t imageCount) {
    m_frameBuffers.resize(imageCount);
    m_frameBufferHashes.resize(imageCount, duk::hash::kUndefinedHash);
}

void VulkanFrameBuffer::clean() {
    for (auto i = 0; i < m_frameBuffers.size(); i++) {
        clean(i);
    }
    m_frameBuffers.clear();
    m_frameBufferHashes.clear();
}

void VulkanFrameBuffer::clean(uint32_t imageIndex) {
    auto& frameBuffer = m_frameBuffers[imageIndex];
    if (frameBuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_device, frameBuffer, nullptr);
        frameBuffer = VK_NULL_HANDLE;
    }
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

void VulkanFrameBuffer::update_extent() {
    m_width = std::numeric_limits<uint32_t>::max();
    m_height = std::numeric_limits<uint32_t>::max();
    for (auto& attachment: m_attachments) {
        m_width = std::min(m_width, attachment->width());
        m_height = std::min(m_height, attachment->height());
    }
}

void VulkanFrameBuffer::update_hash() {
    m_hash = 0;
    for (auto& attachment: m_attachments) {
        duk::hash::hash_combine(m_hash, attachment->hash());
    }
}

}// namespace duk::rhi