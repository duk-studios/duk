/// 23/04/2023
/// vulkan_image.cpp

#include <duk_rhi/vulkan/command/vulkan_command_queue.h>
#include <duk_rhi/vulkan/pipeline/vulkan_pipeline_flags.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_resource_manager.h>

#include <stdexcept>

namespace duk::rhi {

namespace detail {

static VkMemoryPropertyFlags memory_properties_from_update_frequency(Image::UpdateFrequency updateFrequency) {
    VkMemoryPropertyFlags converted;
    switch (updateFrequency) {
        case Image::UpdateFrequency::STATIC:
        case Image::UpdateFrequency::DEVICE_DYNAMIC:
            converted = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case Image::UpdateFrequency::HOST_DYNAMIC:
            converted = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        default:
            throw std::invalid_argument("unhandled Image::UpdateFrequency for VkMemoryPropertyFlags conversion");
    }
    return converted;
}

static VkImageAspectFlags image_aspect(Image::Usage usage, PixelFormat format) {
    VkImageAspectFlags aspectFlags = 0;
    switch (usage) {
        case Image::Usage::COLOR_ATTACHMENT:
            aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
            break;
        case Image::Usage::SAMPLED:
        case Image::Usage::STORAGE:
        case Image::Usage::SAMPLED_STORAGE:
        case Image::Usage::DEPTH_STENCIL_ATTACHMENT:
            if (format.is_depth()) {
                aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
                if (format.is_stencil()) {
                    aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
            } else {
                aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
            }
            break;
        default:
            throw std::invalid_argument("unhandled Image::Usage for VkImageAspectFlags conversion");
    }
    return aspectFlags;
}

}// namespace detail

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

VkImageLayout convert_layout(Image::Layout layout) {
    VkImageLayout converted;
    switch (layout) {
        case Image::Layout::UNDEFINED:
            converted = VK_IMAGE_LAYOUT_UNDEFINED;
            break;
        case Image::Layout::GENERAL:
            converted = VK_IMAGE_LAYOUT_GENERAL;
            break;
        case Image::Layout::COLOR_ATTACHMENT:
            converted = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            break;
        case Image::Layout::DEPTH_ATTACHMENT:
            converted = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            break;
        case Image::Layout::DEPTH_STENCIL_ATTACHMENT:
            converted = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            break;
        case Image::Layout::SHADER_READ_ONLY:
            converted = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            break;
        case Image::Layout::PRESENT_SRC:
            converted = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            break;
        default:
            throw std::invalid_argument("unhandled Image::Layout for Vulkan");
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

void VulkanImage::transition_image_layout(VkCommandBuffer commandBuffer, const TransitionImageLayoutInfo& transitionImageLayoutInfo) {
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = transitionImageLayoutInfo.oldLayout;
    imageMemoryBarrier.newLayout = transitionImageLayoutInfo.newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.subresourceRange = transitionImageLayoutInfo.subresourceRange;
    imageMemoryBarrier.image = transitionImageLayoutInfo.image;

    switch (transitionImageLayoutInfo.oldLayout) {
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
    switch (transitionImageLayoutInfo.newLayout) {
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

    vkCmdPipelineBarrier(commandBuffer, transitionImageLayoutInfo.srcStageMask, transitionImageLayoutInfo.dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VulkanImage::copy_buffer_to_image(VkCommandBuffer commandBuffer, const CopyBufferToImageInfo& copyBufferToImageInfo) {
    TransitionImageLayoutInfo transitionImageLayoutInfo = {};
    transitionImageLayoutInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    transitionImageLayoutInfo.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    transitionImageLayoutInfo.subresourceRange = copyBufferToImageInfo.subresourceRange;
    transitionImageLayoutInfo.image = copyBufferToImageInfo.image;
    transitionImageLayoutInfo.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    transitionImageLayoutInfo.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    transition_image_layout(commandBuffer, transitionImageLayoutInfo);

    VkBufferImageCopy bufferImageCopyRegion = {};
    bufferImageCopyRegion.imageSubresource.aspectMask = copyBufferToImageInfo.subresourceRange.aspectMask;
    bufferImageCopyRegion.imageSubresource.mipLevel = 0;
    bufferImageCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferImageCopyRegion.imageSubresource.layerCount = 1;

    bufferImageCopyRegion.imageOffset = {0, 0, 0};
    bufferImageCopyRegion.imageExtent = {copyBufferToImageInfo.width, copyBufferToImageInfo.height, 1};

    vkCmdCopyBufferToImage(commandBuffer, copyBufferToImageInfo.buffer->handle(), copyBufferToImageInfo.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyRegion);

    transitionImageLayoutInfo.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    transitionImageLayoutInfo.newLayout = copyBufferToImageInfo.finalLayout;
    transitionImageLayoutInfo.subresourceRange = copyBufferToImageInfo.subresourceRange;
    transitionImageLayoutInfo.image = copyBufferToImageInfo.image;
    transitionImageLayoutInfo.srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    transitionImageLayoutInfo.dstStageMask = copyBufferToImageInfo.dstStageMask;

    transition_image_layout(commandBuffer, transitionImageLayoutInfo);
}

VulkanMemoryImage::VulkanMemoryImage(const VulkanMemoryImageCreateInfo& vulkanImageCreateInfo)
    : m_device(vulkanImageCreateInfo.device)
    , m_physicalDevice(vulkanImageCreateInfo.physicalDevice)
    , m_resourceManager(vulkanImageCreateInfo.resourceManager)
    , m_usage(vulkanImageCreateInfo.usage)
    , m_updateFrequency(vulkanImageCreateInfo.updateFrequency)
    , m_layout(vulkanImageCreateInfo.initialLayout)
    , m_format(vulkanImageCreateInfo.imageDataSource->pixel_format())
    , m_dstStage(vulkanImageCreateInfo.dstStages)
    , m_width(vulkanImageCreateInfo.imageDataSource->width())
    , m_height(vulkanImageCreateInfo.imageDataSource->height())
    , m_data(vulkanImageCreateInfo.imageDataSource->byte_count())
    , m_commandQueue(vulkanImageCreateInfo.commandQueue)
    , m_aspectFlags(detail::image_aspect(m_usage, m_format)) {
    if (vulkanImageCreateInfo.imageDataSource->has_data()) {
        vulkanImageCreateInfo.imageDataSource->read_bytes(m_data.data(), m_data.size(), 0);
    }

    create(vulkanImageCreateInfo.imageCount);
}

VulkanMemoryImage::~VulkanMemoryImage() {
    clean();
}

void VulkanMemoryImage::update(uint32_t imageIndex) {
    VulkanBufferMemoryCreateInfo bufferMemoryCreateInfo = {};
    bufferMemoryCreateInfo.commandQueue = m_commandQueue;
    bufferMemoryCreateInfo.device = m_device;
    bufferMemoryCreateInfo.physicalDevice = m_physicalDevice;
    bufferMemoryCreateInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferMemoryCreateInfo.size = m_width * m_height * m_format.size();

    VulkanBufferHostMemory bufferHostMemory(bufferMemoryCreateInfo);
    bufferHostMemory.write(m_data.data(), m_data.size(), 0);

    CopyBufferToImageInfo copyBufferToImageInfo = {};
    copyBufferToImageInfo.buffer = &bufferHostMemory;
    copyBufferToImageInfo.subresourceRange.layerCount = 1;
    copyBufferToImageInfo.subresourceRange.baseArrayLayer = 0;
    copyBufferToImageInfo.subresourceRange.levelCount = 1;
    copyBufferToImageInfo.subresourceRange.baseMipLevel = 0;
    copyBufferToImageInfo.subresourceRange.aspectMask = detail::image_aspect(m_usage, m_format);
    copyBufferToImageInfo.finalLayout = convert_layout(m_layout);
    copyBufferToImageInfo.width = m_width;
    copyBufferToImageInfo.height = m_height;
    copyBufferToImageInfo.dstStageMask = convert_pipeline_stage_mask(m_dstStage);
    copyBufferToImageInfo.image = m_images[imageIndex];

    m_commandQueue->submit([&](VkCommandBuffer commandBuffer) {
        copy_buffer_to_image(commandBuffer, copyBufferToImageInfo);
    });
}

PixelFormat VulkanMemoryImage::format() const {
    return m_format;
}

uint32_t VulkanMemoryImage::width() const {
    return m_width;
}

uint32_t VulkanMemoryImage::height() const {
    return m_height;
}

VkImage VulkanMemoryImage::image(uint32_t imageIndex) const {
    return m_images[imageIndex];
}

VkImageView VulkanMemoryImage::image_view(uint32_t imageIndex) const {
    return m_imageViews[imageIndex];
}

uint32_t VulkanMemoryImage::image_count() const {
    return m_images.size();
}

VkImageAspectFlags VulkanMemoryImage::image_aspect() const {
    return m_aspectFlags;
}

void VulkanMemoryImage::create(uint32_t imageCount) {
    auto format = convert_pixel_format(m_format);

    if (!m_physicalDevice->is_format_supported(format, VK_IMAGE_TILING_OPTIMAL, usage_format_features(m_usage))) {
        throw std::invalid_argument("unsupported image format requested");
    }

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = m_width;
    imageCreateInfo.extent.height = m_height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = convert_usage(m_usage) | (m_data.empty() ? 0 : VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    m_images.resize(imageCount);
    for (auto& image: m_images) {
        auto result = vkCreateImage(m_device, &imageCreateInfo, nullptr, &image);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create VkImage");
        }
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, m_images.front(), &memRequirements);

    auto memoryProperties = detail::memory_properties_from_update_frequency(m_updateFrequency);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_physicalDevice->find_memory_type(memRequirements.memoryTypeBits, memoryProperties);

    m_memories.resize(m_images.size());
    for (int i = 0; i < m_memories.size(); i++) {
        auto result = vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memories[i]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate VkDeviceMemory for VkImage");
        }
        vkBindImageMemory(m_device, m_images[i], m_memories[i], 0);
    }

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.aspectMask = m_aspectFlags;

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = convert_pixel_format(m_format);
    viewInfo.subresourceRange = subresourceRange;

    m_imageViews.resize(imageCount);
    for (int i = 0; i < imageCount; i++) {
        auto& imageView = m_imageViews[i];
        viewInfo.image = m_images[i];

        if (vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    m_resourceManager->schedule_for_update(this);
}

void VulkanMemoryImage::clean() {
    for (int i = 0; i < m_images.size(); i++) {
        clean(i);
    }
    m_images.clear();
    m_imageViews.clear();
    m_memories.clear();
}

void VulkanMemoryImage::clean(uint32_t imageIndex) {
    auto& imageView = m_imageViews[imageIndex];
    if (imageView) {
        vkDestroyImageView(m_device, imageView, nullptr);
        imageView = VK_NULL_HANDLE;
    }

    auto& image = m_images[imageIndex];
    if (image) {
        vkDestroyImage(m_device, image, nullptr);
        image = VK_NULL_HANDLE;
    }

    auto& memory = m_memories[imageIndex];
    if (memory) {
        vkFreeMemory(m_device, memory, nullptr);
        memory = VK_NULL_HANDLE;
    }
}

VulkanSwapchainImage::VulkanSwapchainImage(const VulkanSwapchainImageCreateInfo& vulkanSwapchainImageCreateInfo)
    : m_device(vulkanSwapchainImageCreateInfo.device)
    , m_format(vulkanSwapchainImageCreateInfo.format)
    , m_width(vulkanSwapchainImageCreateInfo.width)
    , m_height(vulkanSwapchainImageCreateInfo.height) {
    auto swapchain = vulkanSwapchainImageCreateInfo.swapchain;

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
        viewInfo.format = m_format;
        viewInfo.subresourceRange = subresourceRange;

        auto result = vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageViews[i]);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create swapchain image view");
        }
    }
}

VulkanSwapchainImage::~VulkanSwapchainImage() {
    for (auto& imageView: m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    m_imageViews.clear();
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

PixelFormat VulkanSwapchainImage::format() const {
    return convert_pixel_format(m_format);
}

uint32_t VulkanSwapchainImage::width() const {
    return m_width;
}

uint32_t VulkanSwapchainImage::height() const {
    return m_height;
}

VkImageAspectFlags VulkanSwapchainImage::image_aspect() const {
    return VK_IMAGE_ASPECT_COLOR_BIT;
}

}// namespace duk::rhi
