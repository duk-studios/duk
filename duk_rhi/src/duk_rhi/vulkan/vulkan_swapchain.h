/// 20/04/2023
/// vulkan_swapchain.h

#ifndef DUK_RHI_VULKAN_SWAPCHAIN_H
#define DUK_RHI_VULKAN_SWAPCHAIN_H

#include <duk_macros/macros.h>
#include <duk_rhi/vulkan/vulkan_events.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>

#include <memory>
#include <vector>

namespace duk {

namespace platform {
class Window;
}

namespace rhi {

class VulkanQueue;
class VulkanImage;

struct VulkanSwapchainCreateInfo {
    VkInstance instance;
    VkDevice device;
    const VulkanPhysicalDevice* physicalDevice;
    const platform::Window* window;
};

class VulkanSwapchain {
public:
    explicit VulkanSwapchain(const VulkanSwapchainCreateInfo& swapchainCreateInfo);

    ~VulkanSwapchain();

    void recreate();

    // -----------------------------------------------------------------------
    // Frame operations
    // -----------------------------------------------------------------------

    VkResult acquire_next_image(VkSemaphore signalSemaphore);

    VkResult present(VulkanQueue& queue, VkSemaphore waitSemaphore) const;

    // -----------------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD uint32_t image_index() const;
    DUK_NO_DISCARD uint32_t image_count() const;
    DUK_NO_DISCARD VkExtent2D extent() const;
    DUK_NO_DISCARD VulkanImage* image(uint32_t imageIndex) const;

private:
    void create_surface();
    void create();
    void clean();

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    const VulkanPhysicalDevice* m_physicalDevice{nullptr};
    const platform::Window* m_window{nullptr};

    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};

    VkSurfaceFormatKHR m_surfaceFormat{};
    VkPresentModeKHR m_presentMode{};
    VkExtent2D m_extent{};

    uint32_t m_imageIndex{};

    std::vector<std::unique_ptr<VulkanImage>> m_images;
};

}// namespace rhi
}// namespace duk

#endif// DUK_RHI_VULKAN_SWAPCHAIN_H
