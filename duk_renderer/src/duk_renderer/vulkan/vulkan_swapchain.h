/// 20/04/2023
/// vulkan_swapchain.h

#ifndef DUK_RENDERER_VULKAN_SWAPCHAIN_H
#define DUK_RENDERER_VULKAN_SWAPCHAIN_H

#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_physical_device.h>

#include <set>

namespace duk::renderer {

struct VulkanSwapchainCreateInfo {
    VkDevice device;
    VkSurfaceKHR surface;
    VulkanPhysicalDevice* physicalDevice;
    std::set<uint32_t> sharedQueueFamilyIndices;
    VkExtent2D extent;
};

class VulkanSwapchain {
public:
    explicit VulkanSwapchain(const VulkanSwapchainCreateInfo& swapchainCreateInfo);

    ~VulkanSwapchain();

    void create();

    void clean();

private:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    VkSurfaceKHR m_surface;
    std::set<uint32_t> m_sharedQueueFamilyIndices;
    VkExtent2D m_requestedExtent;
    VkSwapchainKHR m_swapchain;

    VkSurfaceFormatKHR m_surfaceFormat;
    VkPresentModeKHR m_presentMode;
    VkExtent2D m_extent;
    uint32_t m_imageCount;
};

}

#endif // DUK_RENDERER_VULKAN_SWAPCHAIN_H

