/// 23/04/2023
/// vulkan_image.cpp

#include <duk_rhi/vulkan/vulkan_image.h>

#include <stdexcept>
#include <array>

namespace duk::rhi {

// -----------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------

namespace detail {

static constexpr uint32_t kTransitionBatchSize = 32;

/// Fills one VkImageMemoryBarrier. The subresource range always covers the
/// full mip/layer extent of the image; aspect is derived from the caller.
static VkImageMemoryBarrier fill_image_memory_barrier(
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkImageAspectFlags aspectMask) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    // Source access mask — what was done to the image in the old layout.
    switch (oldLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier.srcAccessMask = 0;
            break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            throw std::logic_error("fill_image_memory_barrier: source layout is not handled");
    }

    // Destination access mask — how the image will be used in the new layout.
    switch (newLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            if (barrier.srcAccessMask == 0) {
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            // Presentation engine reads the image; no explicit access mask needed.
            barrier.dstAccessMask = 0;
            break;
        default:
            // Other target layouts aren't handled yet.
            break;
    }

    return barrier;
}

}

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

void VulkanImage::transition_images_to(VkCommandBuffer commandBuffer, std::span<BulkTransitionEntry> entries) {
    std::array<VkImageMemoryBarrier, detail::kTransitionBatchSize> barriers;
    uint32_t count = 0;
    VkPipelineStageFlags srcStages = 0;
    VkPipelineStageFlags dstStages = 0;

    auto flush = [&]() {
        if (count == 0) {
            return;
        }
        vkCmdPipelineBarrier(commandBuffer,
                             srcStages, dstStages,
                             0, 0, nullptr, 0, nullptr,
                             count, barriers.data());
        count = 0;
        srcStages = 0;
        dstStages = 0;
    };

    for (auto& entry : entries) {
        if (entry.image->m_layout == entry.newLayout) {
            continue;
        }

        barriers[count++] = detail::fill_image_memory_barrier(
                entry.image->m_image,
                entry.image->m_layout,
                entry.newLayout,
                entry.image->m_aspectFlags);

        // Update immediately — no second pass needed.
        entry.image->m_layout = entry.newLayout;
        srcStages |= entry.srcStageMask;
        dstStages |= entry.dstStageMask;

        if (count == detail::kTransitionBatchSize) {
            flush();
        }
    }

    flush();
}

// -----------------------------------------------------------------------
// VulkanImage — memory (owned) constructor
// -----------------------------------------------------------------------

VulkanImage::VulkanImage(const VulkanMemoryImageCreateInfo& createInfo)
    : m_device(createInfo.device)
    , m_format(createInfo.format)
    , m_width(createInfo.width)
    , m_height(createInfo.height)
    , m_aspectFlags(createInfo.aspectFlags)
    , m_ownsImage(true) {

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent = {createInfo.width, createInfo.height, 1};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = createInfo.format;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = createInfo.usageFlags;
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
    allocInfo.memoryTypeIndex = createInfo.physicalDevice->find_memory_type(memReqs.memoryTypeBits, createInfo.memoryFlags);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("VulkanImage: failed to allocate VkDeviceMemory");
    }
    vkBindImageMemory(m_device, m_image, m_memory, 0);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = createInfo.format;
    viewInfo.subresourceRange.aspectMask = createInfo.aspectFlags;
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

VkImageLayout VulkanImage::current_layout() const {
    return m_layout;
}

void VulkanImage::set_layout(VkImageLayout layout) const {
    m_layout = layout;
}

void VulkanImage::transition_to(VkCommandBuffer commandBuffer,
                                 VkImageLayout newLayout,
                                 VkPipelineStageFlags srcStageMask,
                                 VkPipelineStageFlags dstStageMask) {
    if (m_layout == newLayout) {
        return;
    }

    auto barrier = detail::fill_image_memory_barrier(m_image, m_layout, newLayout, m_aspectFlags);
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    m_layout = newLayout;
}

}// namespace duk::rhi
