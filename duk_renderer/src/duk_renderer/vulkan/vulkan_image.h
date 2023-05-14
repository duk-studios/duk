/// 23/04/2023
/// vulkan_image.h

#ifndef DUK_RENDERER_VULKAN_IMAGE_H
#define DUK_RENDERER_VULKAN_IMAGE_H

#include <duk_renderer/image.h>
#include <duk_renderer/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::renderer {

namespace vk {

VkFormat convert_format(ImageFormat format);

ImageFormat convert_format(VkFormat format);

VkImageLayout convert_layout(ImageLayout layout);

}

struct VulkanSwapchainImageCreateInfo {
    VkDevice device;
    VkSwapchainKHR swapchain;
    VkFormat format;
};

class VulkanImage : public Image {
public:

    VulkanImage(const VulkanSwapchainImageCreateInfo& vulkanSwapchainImageCreateInfo);

    ~VulkanImage() override;

    DUK_NO_DISCARD VkImage image(uint32_t frameIndex) const;

    ImageFormat format() const override;

    DUK_NO_DISCARD VkImageView image_view(uint32_t frameIndex) const;

    DUK_NO_DISCARD uint32_t image_count() const;

private:
    VkDevice m_device;
    VkFormat m_format;
    std::vector<VkDeviceMemory> m_memories;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    bool m_createdImages;
};

}

#endif // DUK_RENDERER_VULKAN_IMAGE_H

