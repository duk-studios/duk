/// 23/04/2023
/// vulkan_image.cpp

#include <duk_rhi/vulkan/vulkan_image.h>

#include <stdexcept>

namespace duk::rhi {


VkFormat convert_pixel_format(PixelFormat format) {
    VkFormat converted;
    switch (format) {
        case PixelFormat::UNDEFINED:
            converted = VK_FORMAT_UNDEFINED;
            break;
        case PixelFormat::R8U:
            converted = VK_FORMAT_R8_UNORM;
            break;
        case PixelFormat::R8S:
            converted = VK_FORMAT_R8_SNORM;
            break;
        case PixelFormat::RG8U:
            converted = VK_FORMAT_R8G8_UNORM;
            break;
        case PixelFormat::RG8S:
            converted = VK_FORMAT_R8G8_SNORM;
            break;
        case PixelFormat::RGB8U:
            converted = VK_FORMAT_R8G8B8_UNORM;
            break;
        case PixelFormat::RGB8S:
            converted = VK_FORMAT_R8G8B8_SNORM;
            break;
        case PixelFormat::BGR8U:
            converted = VK_FORMAT_B8G8R8_UNORM;
            break;
        case PixelFormat::BGR8S:
            converted = VK_FORMAT_B8G8R8_SNORM;
            break;
        case PixelFormat::RGBA8U:
            converted = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::RGBA8S:
            converted = VK_FORMAT_R8G8B8A8_SNORM;
            break;
        case PixelFormat::BGRA8U:
            converted = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        case PixelFormat::BGRA8S:
            converted = VK_FORMAT_B8G8R8A8_SNORM;
            break;
        case PixelFormat::RGBA32U:
            converted = VK_FORMAT_R32G32B32A32_UINT;
            break;
        case PixelFormat::RGBA32S:
            converted = VK_FORMAT_R32G32B32A32_SINT;
            break;
        case PixelFormat::RGBA32F:
            converted = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        case PixelFormat::D32_SFLOAT:
            converted = VK_FORMAT_D32_SFLOAT;
            break;
        case PixelFormat::D16_UNORM_S8_UINT:
            converted = VK_FORMAT_D16_UNORM_S8_UINT;
            break;
        case PixelFormat::D24_UNORM_S8_UINT:
            converted = VK_FORMAT_D24_UNORM_S8_UINT;
            break;
        case PixelFormat::D32_SFLOAT_S8_UINT:
            converted = VK_FORMAT_D32_SFLOAT_S8_UINT;
            break;
        default:
            throw std::invalid_argument("unhandled Image::Format for Vulkan");
    }
    return converted;
}

PixelFormat convert_pixel_format(VkFormat format) {
    PixelFormat converted;
    switch (format) {
        case VK_FORMAT_UNDEFINED:
            converted = PixelFormat::UNDEFINED;
            break;
        case VK_FORMAT_R8_UNORM:
            converted = PixelFormat::R8U;
            break;
        case VK_FORMAT_R8_SNORM:
            converted = PixelFormat::R8S;
            break;
        case VK_FORMAT_R8G8_UNORM:
            converted = PixelFormat::RG8U;
            break;
        case VK_FORMAT_R8G8_SNORM:
            converted = PixelFormat::RG8S;
            break;
        case VK_FORMAT_R8G8B8_UNORM:
            converted = PixelFormat::RGB8U;
            break;
        case VK_FORMAT_R8G8B8_SNORM:
            converted = PixelFormat::RGB8S;
            break;
        case VK_FORMAT_B8G8R8_UNORM:
            converted = PixelFormat::BGR8U;
            break;
        case VK_FORMAT_B8G8R8_SNORM:
            converted = PixelFormat::BGR8S;
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            converted = PixelFormat::RGBA8U;
            break;
        case VK_FORMAT_R8G8B8A8_SNORM:
            converted = PixelFormat::RGBA8S;
            break;
        case VK_FORMAT_B8G8R8A8_UNORM:
            converted = PixelFormat::BGRA8U;
            break;
        case VK_FORMAT_B8G8R8A8_SNORM:
            converted = PixelFormat::BGRA8S;
            break;
        case VK_FORMAT_R32G32B32A32_UINT:
            converted = PixelFormat::RGBA32U;
            break;
        case VK_FORMAT_R32G32B32A32_SINT:
            converted = PixelFormat::RGBA32S;
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            converted = PixelFormat::RGBA32F;
            break;
        case VK_FORMAT_D32_SFLOAT:
            converted = PixelFormat::D32_SFLOAT;
            break;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            converted = PixelFormat::D16_UNORM_S8_UINT;
            break;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            converted = PixelFormat::D24_UNORM_S8_UINT;
            break;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            converted = PixelFormat::D32_SFLOAT_S8_UINT;
            break;
        default:
            throw std::invalid_argument("unhandled VkFormat for duk");
    }
    return converted;
}

VkImageUsageFlags convert_usage(Image::Usage usage) {
    VkImageUsageFlags converted;
    switch (usage) {
        case Image::Usage::SAMPLED:
            converted = VK_IMAGE_USAGE_SAMPLED_BIT;
            break;
        case Image::Usage::STORAGE:
            converted = VK_IMAGE_USAGE_STORAGE_BIT;
            break;
        case Image::Usage::SAMPLED_STORAGE:
            converted = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
            break;
        case Image::Usage::COLOR_ATTACHMENT:
            converted = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            break;
        case Image::Usage::DEPTH_STENCIL_ATTACHMENT:
            converted = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            break;
        default:
            throw std::invalid_argument("unhandled Image::Usage for Vulkan");
    }
    return converted;
}

VkFormatFeatureFlags usage_format_features(Image::Usage usage) {
    VkFormatFeatureFlags features = 0;
    switch (usage) {
        case Image::Usage::SAMPLED:
            features |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
            break;
        case Image::Usage::STORAGE:
            features |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
            break;
        case Image::Usage::SAMPLED_STORAGE:
            features |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
            break;
        case Image::Usage::COLOR_ATTACHMENT:
            features |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
            break;
        case Image::Usage::DEPTH_STENCIL_ATTACHMENT:
            features |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
            break;
    }
    return features;
}

void VulkanImage::transition_image_layout(VkCommandBuffer commandBuffer, const TransitionImageLayoutInfo& info) {
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = info.oldLayout;
    imageMemoryBarrier.newLayout = info.newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.subresourceRange = info.subresourceRange;
    imageMemoryBarrier.image = info.image;

    switch (info.oldLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            // Image layout is undefined (or does not matter)
            // Only valid as initial layout
            // No flags required, listed only for completeness
            imageMemoryBarrier.srcAccessMask = 0;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            // Image is preinitialized
            // Only valid as initial layout for linear images, preserves memory contents
            // Make sure host writes have been finished
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            // Image is a color attachment
            // Make sure any writes to the color buffer have been finished
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            // Image is a depth/stencil attachment
            // Make sure any writes to the depth/stencil buffer have been finished
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            // Image is a transfer source
            // Make sure any reads from the image have been finished
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            // Image is a transfer destination
            // Make sure any writes to the image have been finished
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            // Image is read by a shader
            // Make sure any shader reads from the image have been finished
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            // Other source layouts aren't handled (yet)
            throw std::logic_error("image source layout is not handled");
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (info.newLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            // Image will be used as a transfer destination
            // Make sure any writes to the image have been finished
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            // Image will be used as a transfer source
            // Make sure any reads from the image have been finished
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            // Image will be used as a color attachment
            // Make sure any writes to the color buffer have been finished
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            // Image layout will be used as a depth/stencil attachment
            // Make sure any writes to depth/stencil buffer have been finished
            imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            // Image will be read in a shader (sampler, input attachment)
            // Make sure any writes to the image have been finished
            if (imageMemoryBarrier.srcAccessMask == 0) {
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            // Other source layouts aren't handled (yet)
            break;
    }

    vkCmdPipelineBarrier(commandBuffer, info.srcStageMask, info.dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VulkanImage::copy_buffer_to_image(VkCommandBuffer commandBuffer, const CopyBufferToImageInfo& info) {
    TransitionImageLayoutInfo transitionInfo = {};
    transitionInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    transitionInfo.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    transitionInfo.subresourceRange = info.subresourceRange;
    transitionInfo.image = info.image;
    transitionInfo.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    transitionInfo.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    transition_image_layout(commandBuffer, transitionInfo);

    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = info.subresourceRange.aspectMask;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {info.width, info.height, 1};

    vkCmdCopyBufferToImage(commandBuffer, info.buffer, info.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

// -----------------------------------------------------------------------
// VulkanImage — memory (owned) constructor
// -----------------------------------------------------------------------

VulkanImage::VulkanImage(const VulkanMemoryImageCreateInfo& ci)
    : m_device(ci.device)
    , m_format(ci.format)
    , m_width(ci.width)
    , m_height(ci.height)
    , m_aspectFlags(ci.aspectFlags)
    , m_ownsImage(true) {

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent = {ci.width, ci.height, 1};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = ci.format;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = ci.usageFlags;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_device, &imageCreateInfo, nullptr, &m_image) != VK_SUCCESS) {
        throw std::runtime_error("VulkanImage: failed to create VkImage");
    }

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(m_device, m_image, &memReqs);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = ci.physicalDevice->find_memory_type(memReqs.memoryTypeBits, ci.memoryFlags);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("VulkanImage: failed to allocate VkDeviceMemory");
    }
    vkBindImageMemory(m_device, m_image, m_memory, 0);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = ci.format;
    viewInfo.subresourceRange.aspectMask = ci.aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
        throw std::runtime_error("VulkanImage: failed to create VkImageView");
    }
}

// -----------------------------------------------------------------------
// VulkanImage — external (unowned) constructor
// -----------------------------------------------------------------------

VulkanImage::VulkanImage(const VulkanExternalImageCreateInfo& ci)
    : m_device(ci.device)
    , m_format(ci.format)
    , m_width(ci.width)
    , m_height(ci.height)
    , m_aspectFlags(ci.aspectFlags)
    , m_ownsImage(false)
    , m_image(ci.image) {

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = ci.format;
    viewInfo.subresourceRange.aspectMask = ci.aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
        throw std::runtime_error("VulkanImage (external): failed to create VkImageView");
    }
}

VulkanImage::~VulkanImage() {
    if (m_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_device, m_imageView, nullptr);
    }
    if (m_ownsImage) {
        if (m_image != VK_NULL_HANDLE) {
            vkDestroyImage(m_device, m_image, nullptr);
        }
        if (m_memory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_memory, nullptr);
        }
    }
}

PixelFormat VulkanImage::format() const {
    return convert_pixel_format(m_format);
}

uint32_t VulkanImage::width() const {
    return m_width;
}

uint32_t VulkanImage::height() const {
    return m_height;
}

VkImage VulkanImage::image() const {
    return m_image;
}

VkImageView VulkanImage::image_view() const {
    return m_imageView;
}

VkImageAspectFlags VulkanImage::image_aspect() const {
    return m_aspectFlags;
}

}// namespace duk::rhi
