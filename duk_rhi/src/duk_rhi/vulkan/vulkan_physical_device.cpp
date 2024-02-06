//
// Created by Ricardo on 13/04/2023.
//

#include <duk_rhi/vulkan/vulkan_physical_device.h>

#include <stdexcept>

namespace duk::rhi {

namespace detail {

bool is_format_supported(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
        return true;
    }
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
        return true;
    }
    return false;
}

VkFormat find_supported_format(VkPhysicalDevice physicalDevice, std::span<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        if (is_format_supported(physicalDevice, format, tiling, features)) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

}

VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanPhysicalDeviceCreateInfo& physicalDeviceCreateInfo) :
    m_physicalDevice(VK_NULL_HANDLE){
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(physicalDeviceCreateInfo.instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find a physical device on VkInstance");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(physicalDeviceCreateInfo.instance, &deviceCount, devices.data());

    auto desiredGPUIndex = (size_t)physicalDeviceCreateInfo.deviceIndex;
    if (desiredGPUIndex >= devices.size()){
        throw std::runtime_error("physical device index out of bounds");
    }

    m_physicalDevice = devices[desiredGPUIndex];

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

    m_queueFamilyProperties.resize(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);
}

VulkanPhysicalDevice::~VulkanPhysicalDevice() = default;

std::shared_ptr<VulkanQueueFamilyProperties> VulkanPhysicalDevice::find_queue_family(VkSurfaceKHR surface, VkQueueFlags requiredQueueFlags, VkQueueFlags prohibitedQueueFlags) const {
    for (uint32_t candidateQueueIndex = 0; candidateQueueIndex < m_queueFamilyProperties.size(); candidateQueueIndex++) {

        const auto& candidateQueueFamily = m_queueFamilyProperties[candidateQueueIndex];

        if (surface) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, candidateQueueIndex, surface, &presentSupport);

            if (!presentSupport) {
                continue;
            }
        }

        auto candidateFlags = candidateQueueFamily.queueFlags;

        bool hasRequiredFlags = candidateFlags & requiredQueueFlags || requiredQueueFlags == 0;
        bool hasProhibitedFlags = candidateFlags & prohibitedQueueFlags;

        if (candidateQueueFamily.queueCount > 0 && hasRequiredFlags && !hasProhibitedFlags) {
            std::shared_ptr<VulkanQueueFamilyProperties> vulkanQueueFamilyProperties = std::shared_ptr<VulkanQueueFamilyProperties>();
            vulkanQueueFamilyProperties->familyProperties = candidateQueueFamily;
            vulkanQueueFamilyProperties->familyIndex = candidateQueueIndex;
            return vulkanQueueFamilyProperties;
        }
    }

    throw VulkanQueryError(VulkanQueryError::NOT_FOUND);
}

std::shared_ptr<VulkanSurfaceDetails> VulkanPhysicalDevice::query_surface_details(VkSurfaceKHR surface) const {
    if (!surface) {
        throw VulkanQueryError(VulkanQueryError::INVALID_ARGUMENT);
    }

    std::shared_ptr<VulkanSurfaceDetails> surfaceDetails = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &surfaceDetails->capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        surfaceDetails->formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, surfaceDetails->formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        surfaceDetails->presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &presentModeCount, surfaceDetails->presentModes.data());
    }

    return surfaceDetails;
}

VkPhysicalDevice VulkanPhysicalDevice::handle() {
    return m_physicalDevice;
}

const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDevice::queue_family_properties() const {
    return m_queueFamilyProperties;
}

uint32_t VulkanPhysicalDevice::find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1u << i)) && (m_memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type");
}

bool VulkanPhysicalDevice::is_format_supported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) const {
    return detail::is_format_supported(m_physicalDevice, format, tiling, features);
}

VkFormat VulkanPhysicalDevice::select_depth_format(std::span<VkFormat> formats) const {
    return detail::find_supported_format(m_physicalDevice, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

}