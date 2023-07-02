/// 20/04/2023
/// vulkan_swapchain.h

#ifndef DUK_RHI_VULKAN_SWAPCHAIN_H
#define DUK_RHI_VULKAN_SWAPCHAIN_H

#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/command/vulkan_command.h>
#include <duk_rhi/vulkan/vulkan_events.h>

#include <set>
#include <memory>

namespace duk {

namespace platform {
class Window;
}

namespace rhi {

class VulkanSwapchainImage;

struct VulkanImageAcquireCommandCreateInfo {
    VkDevice device;
    uint32_t frameCount;
    const uint32_t* currentFramePtr;
};

class VulkanImageAcquireCommand : public Command {
public:

    explicit VulkanImageAcquireCommand(const VulkanImageAcquireCommandCreateInfo& commandCreateInfo);

    void submit(const VulkanCommandParams& commandParams);

    Submitter* submitter_ptr() override;

private:
    VulkanSubmitter m_submitter;
};

struct VulkanPresentCommandCreateInfo {
    VkQueue presentQueue;
    uint32_t* currentImagePtr;
    bool* requiresRecreationPtr;
    bool* ableToPresentPtr;
    VulkanSwapchainCreateEvent* swapchainCreateEvent;
};

class VulkanPresentCommand : public Command {
public:

    explicit VulkanPresentCommand(const VulkanPresentCommandCreateInfo& commandCreateInfo);

    void submit(const VulkanCommandParams& commandParams);

    DUK_NO_DISCARD Submitter* submitter_ptr() override;

private:
    VkSwapchainKHR m_swapchain;
    VkQueue m_presentQueue;
    uint32_t* m_currentImagePtr;
    bool* m_requiresRecreationPtr;
    bool* m_ableToPresentPtr;
    VulkanSubmitter m_submitter;
    events::EventListener m_listener;
};

struct VulkanSwapchainCreateInfo {
    VkDevice device;
    VkSurfaceKHR surface;
    platform::Window* window;
    VulkanPhysicalDevice* physicalDevice;
    VkQueue presentQueue;
    VulkanPrepareFrameEvent* prepareFrameEvent;
    uint32_t frameCount;
    const uint32_t* currentFramePtr;
};

class VulkanSwapchain {
public:
    explicit VulkanSwapchain(const VulkanSwapchainCreateInfo& swapchainCreateInfo);

    ~VulkanSwapchain();

    void create();

    void clean();

    VulkanImageAcquireCommand* acquire_image_command();

    VulkanPresentCommand* present_command();

    DUK_NO_DISCARD const uint32_t* current_image_ptr() const;

    DUK_NO_DISCARD uint32_t image_count() const;

    DUK_NO_DISCARD VkExtent2D extent() const;

    DUK_NO_DISCARD VulkanSwapchainImage* image() const;

    DUK_NO_DISCARD VulkanSwapchainCreateEvent* create_event();

    DUK_NO_DISCARD VulkanSwapchainCleanEvent* clean_event();

private:

    void recreate_swapchain();

private:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    VkSurfaceKHR m_surface;
    platform::Window* m_window;
    VkQueue m_presentQueue;
    VkSwapchainKHR m_swapchain;

    VkSurfaceFormatKHR m_surfaceFormat;
    VkPresentModeKHR m_presentMode;
    VkExtent2D m_extent;

    duk::events::EventListener m_listener;

    uint32_t m_currentImage{};
    std::unique_ptr<VulkanSwapchainImage> m_image;
    std::unique_ptr<VulkanImageAcquireCommand> m_acquireImageCommand;
    std::unique_ptr<VulkanPresentCommand> m_presentCommand;
    VulkanSwapchainCreateEvent m_swapchainCreateEvent;
    VulkanSwapchainCleanEvent m_swapchainCleanEvent;
    bool m_requiresRecreation;
    bool m_ableToPresent;
};

} // namespace rhi
} // namespace duk

#endif // DUK_RHI_VULKAN_SWAPCHAIN_H

