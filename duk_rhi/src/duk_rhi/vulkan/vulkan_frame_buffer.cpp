/// 23/04/2023
/// vulkan_frame_buffer.cpp

#include <duk_rhi/vulkan/vulkan_frame_buffer.h>

#include <limits>

namespace duk::rhi {

void VulkanFrameBuffer::write(const VulkanImage* const* attachments, uint32_t attachmentCount) {
    m_attachments.assign(attachments, attachments + attachmentCount);
    m_width = std::numeric_limits<uint32_t>::max();
    m_height = std::numeric_limits<uint32_t>::max();
    for (auto& attachment : m_attachments) {
        m_width = std::min(m_width, attachment->width());
        m_height = std::min(m_height, attachment->height());
    }
    if (m_attachments.empty()) {
        m_width = 0;
        m_height = 0;
    }
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

const Image* VulkanFrameBuffer::at(uint32_t attachment) const {
    return m_attachments.at(attachment);
}

}// namespace duk::rhi

