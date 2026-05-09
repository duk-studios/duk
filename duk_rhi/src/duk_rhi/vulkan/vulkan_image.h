/// 23/04/2023
/// vulkan_image.h

#ifndef DUK_RHI_VULKAN_IMAGE_H
#define DUK_RHI_VULKAN_IMAGE_H

#include <duk_rhi/image.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>

#include <duk_macros/macros.h>

namespace duk::rhi {

VkFormat convert_pixel_format(PixelFormat format);

PixelFormat convert_pixel_format(VkFormat format);

VkImageUsageFlags convert_usage(Image::Usage usage);

VkFormatFeatureFlags usage_format_features(Image::Usage usage);

/// For images whose VkImage + VkDeviceMemory are owned by this instance.
/// All members are native Vulkan types; the command context converts
/// public API enums before filling this struct.
struct VulkanMemoryImageCreateInfo {
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    VkFormat format;
    uint32_t width;
    uint32_t height;
    VkImageUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryFlags;
    VkImageAspectFlags aspectFlags;
};

/// For externally-owned images (e.g. swapchain images).
/// VulkanImage creates a VkImageView but does NOT free the VkImage.
struct VulkanExternalImageCreateInfo {
    VkDevice device;
    VkImage image;   ///< Externally managed — not freed by VulkanImage.
    VkFormat format;
    uint32_t width;
    uint32_t height;
    VkImageAspectFlags aspectFlags;
};

class VulkanImage : public Image {
public:
    explicit VulkanImage(const VulkanMemoryImageCreateInfo& ci);

    explicit VulkanImage(const VulkanExternalImageCreateInfo& ci);

    ~VulkanImage() override;

    // -----------------------------------------------------------------------
    // Image public interface (read-only metadata)
    // -----------------------------------------------------------------------

    DUK_NO_DISCARD PixelFormat format() const override;

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    // -----------------------------------------------------------------------
    // Internal Vulkan accessors
    // -----------------------------------------------------------------------

    DUK_NO_DISCARD VkImage image() const;

    DUK_NO_DISCARD VkImageView image_view() const;

    DUK_NO_DISCARD VkImageAspectFlags image_aspect() const;

    // -----------------------------------------------------------------------
    // Static utility methods
    // -----------------------------------------------------------------------

    struct TransitionImageLayoutInfo {
        VkImageLayout oldLayout;
        VkImageLayout newLayout;
        VkImageSubresourceRange subresourceRange;
        VkPipelineStageFlags srcStageMask;
        VkPipelineStageFlags dstStageMask;
        VkImage image;
    };

    static void transition_image_layout(VkCommandBuffer commandBuffer, const TransitionImageLayoutInfo& info);

    struct CopyBufferToImageInfo {
        VkBuffer buffer;
        VkImage image;
        uint32_t width;
        uint32_t height;
        VkImageSubresourceRange subresourceRange;
    };

    static void copy_buffer_to_image(VkCommandBuffer commandBuffer, const CopyBufferToImageInfo& info);

private:
    VkDevice m_device;
    VkFormat m_format;
    uint32_t m_width;
    uint32_t m_height;
    VkImageAspectFlags m_aspectFlags;
    bool m_ownsImage{false}; ///< True when VkImage + VkDeviceMemory were allocated here.
    VkImage m_image{VK_NULL_HANDLE};
    VkImageView m_imageView{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE}; ///< VK_NULL_HANDLE for external images.
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_IMAGE_H
