/// 20/04/2023
/// vulkan_swapchain.h

#ifndef DUK_RHI_VULKAN_SWAPCHAIN_H
#define DUK_RHI_VULKAN_SWAPCHAIN_H

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

class Image;
class VulkanSwapchainImage;

/// Per-frame binary semaphores owned by the swapchain for acquire/present.
/// Fences for CPU-GPU synchronisation live in VulkanCommandContext.
struct VulkanFrameSync {
    VkSemaphore imageAvailableSemaphore{VK_NULL_HANDLE};
    VkSemaphore renderFinishedSemaphore{VK_NULL_HANDLE};
};

struct VulkanSwapchainCreateInfo {
    VkInstance instance;
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    platform::Window* window;
    VkQueue presentQueue;
    uint32_t framesInFlight;
};

class VulkanSwapchain {
public:
    explicit VulkanSwapchain(const VulkanSwapchainCreateInfo& swapchainCreateInfo);

    ~VulkanSwapchain();

    void recreate();

    // -----------------------------------------------------------------------
    // Frame operations
    // -----------------------------------------------------------------------

    /// Waits for the in-flight fence, acquires the next swapchain image, and
    /// signals imageAvailableSemaphore. Returns the acquired image index.
    uint32_t acquire_next_image(uint32_t frameIndex);

    /// Submits vkQueuePresentKHR, waiting on renderFinishedSemaphore.
    void present(uint32_t imageIndex, uint32_t frameIndex);

    DUK_NO_DISCARD VulkanFrameSync& frame_sync(uint32_t frameIndex);

    // -----------------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD const uint32_t* current_image_ptr() const;
    DUK_NO_DISCARD uint32_t image_count() const;
    DUK_NO_DISCARD VkExtent2D extent() const;
    DUK_NO_DISCARD VkSwapchainKHR handle() const;
    DUK_NO_DISCARD Image* image() const;
    DUK_NO_DISCARD VkSurfaceKHR vk_surface() const;

    DUK_NO_DISCARD VulkanSwapchainCreateEvent* create_event();
    DUK_NO_DISCARD VulkanSwapchainCleanEvent* clean_event();

private:
    void create_platform_surface();
    void create();
    void clean();
    void create_frame_sync();
    void destroy_frame_sync();

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    VulkanPhysicalDevice* m_physicalDevice{nullptr};
    platform::Window* m_window{nullptr};
    VkQueue m_presentQueue{VK_NULL_HANDLE};
    uint32_t m_framesInFlight{0};

    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};

    VkSurfaceFormatKHR m_surfaceFormat{};
    VkPresentModeKHR m_presentMode{};
    VkExtent2D m_extent{};

    uint32_t m_currentImage{};
    bool m_requiresRecreation{false};
    bool m_ableToPresent{false};

    std::unique_ptr<VulkanSwapchainImage> m_image;
    std::vector<VulkanFrameSync> m_frameSync;
    duk::event::Listener m_listener;

    VulkanSwapchainCreateEvent m_swapchainCreateEvent;
    VulkanSwapchainCleanEvent m_swapchainCleanEvent;
};

}// namespace rhi
}// namespace duk

#endif// DUK_RHI_VULKAN_SWAPCHAIN_H
