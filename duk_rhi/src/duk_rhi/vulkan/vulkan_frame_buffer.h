/// 23/04/2023
/// vulkan_frame_buffer.h

#ifndef DUK_RHI_VULKAN_FRAME_BUFFER_H
#define DUK_RHI_VULKAN_FRAME_BUFFER_H

#include <duk_rhi/frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_deletion_queue.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <vector>

namespace duk::rhi {

class VulkanFrameBuffer : public FrameBuffer {
public:

    void write(const VulkanImage* const* attachments, uint32_t attachmentCount);

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD glm::uvec2 size() const override;

    DUK_NO_DISCARD const Image* at(uint32_t attachment) const override;

private:
    std::vector<const VulkanImage*> m_attachments;
    uint32_t m_width{0};
    uint32_t m_height{0};
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_FRAME_BUFFER_H
