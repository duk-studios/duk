/// 23/04/2023
/// vulkan_image.cpp

#include <duk_renderer/vulkan/vulkan_image.h>
#include <stdexcept>

namespace duk::renderer {

namespace vk {

VkFormat convert_format(ImageFormat format) {
    VkFormat converted;
    switch (format){
        case ImageFormat::UNDEFINED: converted = VK_FORMAT_UNDEFINED; break;
        case ImageFormat::R8_UNORM: converted = VK_FORMAT_R8_UNORM; break;
        case ImageFormat::R8_SNORM: converted = VK_FORMAT_R8_SNORM; break;
        case ImageFormat::R8_SRGB: converted = VK_FORMAT_R8_SRGB; break;
        case ImageFormat::R8G8_UNORM: converted = VK_FORMAT_R8G8_UNORM; break;
        case ImageFormat::R8G8_SNORM: converted = VK_FORMAT_R8G8_SNORM; break;
        case ImageFormat::R8G8_SRGB: converted = VK_FORMAT_R8G8_SRGB; break;
        case ImageFormat::R8G8B8_UNORM: converted = VK_FORMAT_R8G8B8_UNORM; break;
        case ImageFormat::R8G8B8_SNORM: converted = VK_FORMAT_R8G8B8_SNORM; break;
        case ImageFormat::R8G8B8_SRGB: converted = VK_FORMAT_R8G8B8_SRGB; break;
        case ImageFormat::B8G8R8_UNORM: converted = VK_FORMAT_B8G8R8_UNORM; break;
        case ImageFormat::B8G8R8_SNORM: converted = VK_FORMAT_B8G8R8_SNORM; break;
        case ImageFormat::B8G8R8_SRGB: converted = VK_FORMAT_B8G8R8_SRGB; break;
        case ImageFormat::R8G8B8A8_UNORM: converted = VK_FORMAT_R8G8B8A8_UNORM; break;
        case ImageFormat::R8G8B8A8_SNORM: converted = VK_FORMAT_R8G8B8A8_SNORM; break;
        case ImageFormat::R8G8B8A8_SRGB: converted = VK_FORMAT_R8G8B8A8_SRGB; break;
        case ImageFormat::B8G8R8A8_UNORM: converted = VK_FORMAT_B8G8R8A8_UNORM; break;
        case ImageFormat::B8G8R8A8_SNORM: converted = VK_FORMAT_B8G8R8A8_SNORM; break;
        case ImageFormat::B8G8R8A8_SRGB: converted = VK_FORMAT_B8G8R8A8_SRGB; break;
        default:
            throw std::invalid_argument("unhandled ImageFormat for Vulkan");
    }
    return converted;
}

ImageFormat convert_format(VkFormat format) {
    ImageFormat converted;
    switch (format){
        case VK_FORMAT_UNDEFINED: converted = ImageFormat::UNDEFINED; break;
        case VK_FORMAT_R8_UNORM: converted = ImageFormat::R8_UNORM; break;
        case VK_FORMAT_R8_SNORM: converted = ImageFormat::R8_SNORM; break;
        case VK_FORMAT_R8_SRGB: converted = ImageFormat::R8_SRGB; break;
        case VK_FORMAT_R8G8_UNORM: converted = ImageFormat::R8G8_UNORM; break;
        case VK_FORMAT_R8G8_SNORM: converted = ImageFormat::R8G8_SNORM; break;
        case VK_FORMAT_R8G8_SRGB: converted = ImageFormat::R8G8_SRGB; break;
        case VK_FORMAT_R8G8B8_UNORM: converted = ImageFormat::R8G8B8_UNORM; break;
        case VK_FORMAT_R8G8B8_SNORM: converted = ImageFormat::R8G8B8_SNORM; break;
        case VK_FORMAT_R8G8B8_SRGB: converted = ImageFormat::R8G8B8_SRGB; break;
        case VK_FORMAT_B8G8R8_UNORM: converted = ImageFormat::B8G8R8_UNORM; break;
        case VK_FORMAT_B8G8R8_SNORM: converted = ImageFormat::B8G8R8_SNORM; break;
        case VK_FORMAT_B8G8R8_SRGB: converted = ImageFormat::B8G8R8_SRGB; break;
        case VK_FORMAT_R8G8B8A8_UNORM: converted = ImageFormat::R8G8B8A8_UNORM; break;
        case VK_FORMAT_R8G8B8A8_SNORM: converted = ImageFormat::R8G8B8A8_SNORM; break;
        case VK_FORMAT_R8G8B8A8_SRGB: converted = ImageFormat::R8G8B8A8_SRGB; break;
        case VK_FORMAT_B8G8R8A8_UNORM: converted = ImageFormat::B8G8R8A8_UNORM; break;
        case VK_FORMAT_B8G8R8A8_SNORM: converted = ImageFormat::B8G8R8A8_SNORM; break;
        case VK_FORMAT_B8G8R8A8_SRGB: converted = ImageFormat::B8G8R8A8_SRGB; break;
        default:
            throw std::invalid_argument("unhandled VkFormat for duk");
    }
    return converted;
}

VkImageLayout convert_layout(ImageLayout layout) {
    VkImageLayout converted;
    switch (layout){
        case ImageLayout::UNDEFINED: converted = VK_IMAGE_LAYOUT_UNDEFINED; break;
        case ImageLayout::GENERAL: converted = VK_IMAGE_LAYOUT_GENERAL; break;
        case ImageLayout::COLOR_ATTACHMENT: converted = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; break;
        case ImageLayout::DEPTH_ATTACHMENT: converted = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL; break;
        case ImageLayout::SHADER_READ_ONLY: converted = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; break;
        case ImageLayout::PRESENT_SRC: converted = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; break;
        default:
            throw std::invalid_argument("unhandled ImageLayout for Vulkan");
    }
    return converted;
}

}

VulkanMemoryImage::VulkanMemoryImage(const VulkanMemoryImageCreateInfo& vulkanImageCreateInfo) {

}

VulkanMemoryImage::~VulkanMemoryImage() {
    for (auto& imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    m_imageViews.clear();

    for (auto& image : m_images){
        vkDestroyImage(m_device, image, nullptr);
    }
    m_images.clear();

    for (auto& memory : m_memories){
        vkFreeMemory(m_device, memory, nullptr);
    }
    m_memories.clear();
}

ImageFormat VulkanMemoryImage::format() const {
    return ImageFormat::UNDEFINED;
}

uint32_t VulkanMemoryImage::width() const {
    return 0;
}

uint32_t VulkanMemoryImage::height() const {
    return 0;
}

VkImage VulkanMemoryImage::image(uint32_t frameIndex) const {
    return nullptr;
}

VkImageView VulkanMemoryImage::image_view(uint32_t frameIndex) const {
    return nullptr;
}

uint32_t VulkanMemoryImage::image_count() const {
    return 0;
}

void VulkanMemoryImage::create(uint32_t imageCount) {

}

void VulkanMemoryImage::clean() {

}

void VulkanMemoryImage::clean(uint32_t imageIndex) {

}

VulkanSwapchainImage::VulkanSwapchainImage(const VulkanSwapchainImageCreateInfo& vulkanSwapchainImageCreateInfo) :
    m_device(vulkanSwapchainImageCreateInfo.device),
    m_format(VK_FORMAT_UNDEFINED) {

}

VulkanSwapchainImage::~VulkanSwapchainImage() {
    clean();
}

VkImage VulkanSwapchainImage::image(uint32_t frameIndex) const {
    return m_images[frameIndex];
}

VkImageView VulkanSwapchainImage::image_view(uint32_t frameIndex) const {
    return m_imageViews[frameIndex];
}

uint32_t VulkanSwapchainImage::image_count() const {
    return m_images.size();
}

ImageFormat VulkanSwapchainImage::format() const {
    return vk::convert_format(m_format);
}

uint32_t VulkanSwapchainImage::width() const {
    return m_width;
}

uint32_t VulkanSwapchainImage::height() const {
    return m_height;
}

void VulkanSwapchainImage::create(VkFormat format, uint32_t width, uint32_t height, VkSwapchainKHR swapchain) {
    m_format = format;
    m_width = width;
    m_height = height;

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(m_device, swapchain, &imageCount, nullptr);

    m_images.resize(imageCount);

    vkGetSwapchainImagesKHR(m_device, swapchain, &imageCount, m_images.data());

    m_imageViews.resize(imageCount);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    for (uint32_t i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange = subresourceRange;

        auto result = vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageViews[i]);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create swapchain image view");
        }
    }
}

void VulkanSwapchainImage::clean() {
    for (auto& imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    m_imageViews.clear();
}

}

