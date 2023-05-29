/// 23/04/2023
/// vulkan_image.h

#ifndef DUK_RENDERER_VULKAN_IMAGE_H
#define DUK_RENDERER_VULKAN_IMAGE_H

#include <duk_renderer/image.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_events.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::renderer {

namespace vk {

VkFormat convert_format(ImageFormat format);

ImageFormat convert_format(VkFormat format);

VkImageLayout convert_layout(ImageLayout layout);

}

class VulkanImage : public Image {
public:
    DUK_NO_DISCARD virtual VkImage image(uint32_t frameIndex) const = 0;

    DUK_NO_DISCARD virtual VkImageView image_view(uint32_t frameIndex) const = 0;

    DUK_NO_DISCARD virtual uint32_t image_count() const = 0;
};

struct VulkanMemoryImageCreateInfo {
    VkDevice device;
    VkFormat format;
};

class VulkanMemoryImage : public VulkanImage {
public:

    explicit VulkanMemoryImage(const VulkanMemoryImageCreateInfo& vulkanImageCreateInfo);

    ~VulkanMemoryImage() override;

    DUK_NO_DISCARD ImageFormat format() const override;

    DUK_NO_DISCARD VkImage image(uint32_t frameIndex) const override;

    DUK_NO_DISCARD VkImageView image_view(uint32_t frameIndex) const override;

    DUK_NO_DISCARD uint32_t image_count() const override;

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    void create(uint32_t imageCount);

    void clean();

    void clean(uint32_t imageIndex);

private:
    VkDevice m_device;
    std::vector<VkDeviceMemory> m_memories;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

struct VulkanSwapchainImageCreateInfo {
    VkDevice device;
};

class VulkanSwapchainImage : public VulkanImage {
public:

    explicit VulkanSwapchainImage(const VulkanSwapchainImageCreateInfo& vulkanSwapchainImageCreateInfo);

    ~VulkanSwapchainImage() override;

    DUK_NO_DISCARD ImageFormat format() const override;

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD VkImage image(uint32_t frameIndex) const override;

    DUK_NO_DISCARD VkImageView image_view(uint32_t frameIndex) const override;

    DUK_NO_DISCARD uint32_t image_count() const override;

    void create(VkFormat format, uint32_t width, uint32_t height, VkSwapchainKHR swapchain);

    void clean();

private:
    VkDevice m_device;
    VkFormat m_format;
    uint32_t m_width;
    uint32_t m_height;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

}

#endif // DUK_RENDERER_VULKAN_IMAGE_H

