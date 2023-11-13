/// 23/04/2023
/// vulkan_image.h

#ifndef DUK_RHI_VULKAN_IMAGE_H
#define DUK_RHI_VULKAN_IMAGE_H

#include <duk_rhi/image.h>
#include <duk_rhi/image_data_source.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_events.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::rhi {

VkFormat convert_pixel_format(PixelFormat format);

PixelFormat convert_pixel_format(VkFormat format);

VkImageLayout convert_layout(Image::Layout layout);

VkImageUsageFlags convert_usage(Image::Usage usage);

class VulkanImage : public Image {
public:

    virtual void update(uint32_t imageIndex, VkPipelineStageFlags stageFlags) = 0;

    DUK_NO_DISCARD virtual VkImage image(uint32_t imageIndex) const = 0;

    DUK_NO_DISCARD virtual VkImageView image_view(uint32_t imageIndex) const = 0;

    DUK_NO_DISCARD virtual uint32_t image_count() const = 0;

    DUK_NO_DISCARD virtual VkImageAspectFlags image_aspect() const = 0;

    struct TransitionImageLayoutInfo {
        VkImageLayout oldLayout;
        VkImageLayout newLayout;
        VkImageSubresourceRange subresourceRange;
        VkPipelineStageFlags srcStageMask;
        VkPipelineStageFlags dstStageMask;
        VkImage image;
    };

    static void transition_image_layout(VkCommandBuffer commandBuffer, const TransitionImageLayoutInfo& transitionImageLayoutInfo);

    struct CopyBufferToImageInfo {
        VulkanBufferMemory* buffer;
        VkImage image;
        uint32_t width;
        uint32_t height;
        VkImageLayout finalLayout;
        VkPipelineStageFlags dstStageMask;
        VkImageSubresourceRange subresourceRange;
    };

    static void copy_buffer_to_image(VkCommandBuffer commandBuffer, const CopyBufferToImageInfo& copyBufferToImageInfo);
};

struct VulkanMemoryImageCreateInfo {
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    uint32_t imageCount;
    Image::Usage usage;
    Image::UpdateFrequency updateFrequency;
    Image::Layout initialLayout;
    VulkanCommandQueue* commandQueue;
    ImageDataSource* imageDataSource;
};

class VulkanMemoryImage : public VulkanImage {
public:

    explicit VulkanMemoryImage(const VulkanMemoryImageCreateInfo& vulkanImageCreateInfo);

    ~VulkanMemoryImage() override;

    void update(uint32_t imageIndex, VkPipelineStageFlags stageFlags) override;

    void update(ImageDataSource* imageDataSource) override;

    DUK_NO_DISCARD PixelFormat format() const override;

    DUK_NO_DISCARD VkImage image(uint32_t imageIndex) const override;

    DUK_NO_DISCARD VkImageView image_view(uint32_t imageIndex) const override;

    DUK_NO_DISCARD uint32_t image_count() const override;

    DUK_NO_DISCARD VkImageAspectFlags image_aspect() const override;

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD duk::hash::Hash hash() const override;

    void create(uint32_t imageCount);

    void clean();

    void clean(uint32_t imageIndex);
private:

    DUK_NO_DISCARD uint32_t fix_index(uint32_t imageIndex) const;

private:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    Usage m_usage;
    UpdateFrequency m_updateFrequency;
    Layout m_layout;
    PixelFormat m_format;
    uint32_t m_width;
    uint32_t m_height;
    std::vector<uint8_t> m_data;
    duk::hash::Hash m_dataSourceHash;
    VulkanCommandQueue* m_commandQueue;
    VkImageAspectFlags m_aspectFlags;

    std::vector<VkDeviceMemory> m_memories;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<duk::hash::Hash> m_imageDataHashes;
};

struct VulkanSwapchainImageCreateInfo {
    VkDevice device;
};

class VulkanSwapchainImage : public VulkanImage {
public:

    explicit VulkanSwapchainImage(const VulkanSwapchainImageCreateInfo& vulkanSwapchainImageCreateInfo);

    ~VulkanSwapchainImage() override;

    void update(uint32_t imageIndex, VkPipelineStageFlags stageFlags) override;

    void update(ImageDataSource* imageDataSource) override;

    DUK_NO_DISCARD PixelFormat format() const override;

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD VkImage image(uint32_t frameIndex) const override;

    DUK_NO_DISCARD VkImageView image_view(uint32_t frameIndex) const override;

    DUK_NO_DISCARD uint32_t image_count() const override;

    DUK_NO_DISCARD VkImageAspectFlags image_aspect() const override;

    DUK_NO_DISCARD duk::hash::Hash hash() const override;

    void create(VkFormat format, uint32_t width, uint32_t height, VkSwapchainKHR swapchain);

    void clean();

private:
    VkDevice m_device;
    VkFormat m_format;
    uint32_t m_width;
    uint32_t m_height;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    duk::hash::Hash m_hash;
};

}

#endif // DUK_RHI_VULKAN_IMAGE_H

