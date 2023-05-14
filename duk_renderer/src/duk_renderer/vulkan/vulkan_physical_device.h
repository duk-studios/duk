//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_VULKAN_PHYSICAL_DEVICE_H
#define DUK_VULKAN_PHYSICAL_DEVICE_H

#include <duk_macros/macros.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_query_error.h>

#include <tl/expected.hpp>

namespace duk::renderer {

struct VulkanQueueFamilyProperties {
    VkQueueFamilyProperties familyProperties;
    uint32_t familyIndex;
};

using ExpectedVulkanQueueFamilyProperties = tl::expected<VulkanQueueFamilyProperties, VulkanQueryError>;

struct VulkanPhysicalDeviceCreateInfo {
    VkInstance instance;
    uint32_t deviceIndex;
};

class VulkanPhysicalDevice {
public:
    explicit VulkanPhysicalDevice(const VulkanPhysicalDeviceCreateInfo& physicalDeviceCreateInfo);
    ~VulkanPhysicalDevice();

    ExpectedVulkanQueueFamilyProperties query_queue_family_properties(VkSurfaceKHR surface, VkQueueFlags requiredQueueFlags, VkQueueFlags prohibitedQueueFlags = 0);

    DUK_NO_DISCARD VkPhysicalDevice handle();

    DUK_NO_DISCARD const VkPhysicalDevice handle() const;

private:
    VkPhysicalDevice m_physicalDevice;
    std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
};

}

#endif //DUK_VULKAN_PHYSICAL_DEVICE_H
