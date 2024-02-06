//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_VULKAN_PHYSICAL_DEVICE_H
#define DUK_VULKAN_PHYSICAL_DEVICE_H

#include <duk_macros/macros.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_query_error.h>

#include <span>
#include <vector>
#include <memory>

namespace duk::rhi {

struct VulkanQueueFamilyProperties {
    VkQueueFamilyProperties familyProperties;
    uint32_t familyIndex;
};

struct VulkanSurfaceDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanPhysicalDeviceCreateInfo {
    VkInstance instance;
    uint32_t deviceIndex;
};

class VulkanPhysicalDevice {
public:
    explicit VulkanPhysicalDevice(const VulkanPhysicalDeviceCreateInfo& physicalDeviceCreateInfo);

    ~VulkanPhysicalDevice();

    DUK_NO_DISCARD std::shared_ptr<VulkanQueueFamilyProperties> find_queue_family(VkSurfaceKHR surface, VkQueueFlags requiredQueueFlags, VkQueueFlags prohibitedQueueFlags = 0) const;

    DUK_NO_DISCARD std::shared_ptr<VulkanSurfaceDetails> query_surface_details(VkSurfaceKHR surface) const;

    DUK_NO_DISCARD VkPhysicalDevice handle();

    DUK_NO_DISCARD const std::vector<VkQueueFamilyProperties>& queue_family_properties() const;

    DUK_NO_DISCARD uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    DUK_NO_DISCARD bool is_format_supported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    DUK_NO_DISCARD VkFormat select_depth_format(std::span<VkFormat> formats) const;

private:
    VkPhysicalDevice m_physicalDevice;
    std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
    VkPhysicalDeviceMemoryProperties m_memoryProperties;
};

}

#endif //DUK_VULKAN_PHYSICAL_DEVICE_H
