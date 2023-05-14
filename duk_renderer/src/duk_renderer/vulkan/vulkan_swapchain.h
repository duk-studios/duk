/// 20/04/2023
/// vulkan_swapchain.h

#ifndef DUK_RENDERER_VULKAN_SWAPCHAIN_H
#define DUK_RENDERER_VULKAN_SWAPCHAIN_H

#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_physical_device.h>
#include <duk_renderer/vulkan/vulkan_command.h>

#include <set>
#include <memory>

namespace duk::renderer {

class VulkanImage;

struct VulkanImageAcquireCommandCreateInfo {
    VkDevice device;
    VkSwapchainKHR swapchain;
    uint32_t* currentImagePtr;
};

class VulkanImageAcquireCommand : public Command, public VulkanCommand {
public:
    explicit VulkanImageAcquireCommand(const VulkanImageAcquireCommandCreateInfo& commandCreateInfo);
    void submit(const VulkanCommandParams& commandParams) override;
private:
    VkDevice m_device;
    VkSwapchainKHR m_swapchain;
    uint32_t* m_currentImagePtr;
};

struct VulkanPresentCommandCreateInfo {
    VkDevice device;
    VkSwapchainKHR swapchain;
    uint32_t* currentImagePtr;
    VkQueue presentQueue;
};

class VulkanPresentCommand : public Command, public VulkanCommand {
public:
    explicit VulkanPresentCommand(const VulkanPresentCommandCreateInfo& commandCreateInfo);
    void submit(const VulkanCommandParams& commandParams) override;
private:
    VkSwapchainKHR m_swapchain;
    VkQueue m_presentQueue;
    uint32_t* m_currentImagePtr;
};

struct VulkanSwapchainCreateInfo {
    VkDevice device;
    VkSurfaceKHR surface;
    VulkanPhysicalDevice* physicalDevice;
    VkExtent2D extent;
    VkQueue presentQueue;
};

class VulkanSwapchain {
public:
    explicit VulkanSwapchain(const VulkanSwapchainCreateInfo& swapchainCreateInfo);

    ~VulkanSwapchain();

    void create();

    void clean();

    VulkanImageAcquireCommand* acquire_image_command();

    VulkanPresentCommand* present_command();

    DUK_NO_DISCARD uint32_t current_image() const;

    DUK_NO_DISCARD uint32_t image_count() const;

    DUK_NO_DISCARD VkExtent2D extent() const;

    DUK_NO_DISCARD VulkanImage* image() const;


private:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    VkSurfaceKHR m_surface;
    VkExtent2D m_requestedExtent;
    VkQueue m_presentQueue;
    VkSwapchainKHR m_swapchain;

    VkSurfaceFormatKHR m_surfaceFormat;
    VkPresentModeKHR m_presentMode;
    VkExtent2D m_extent;
    uint32_t m_currentImage;

    std::unique_ptr<VulkanImage> m_image;
    std::unique_ptr<VulkanImageAcquireCommand> m_acquireImageCommand;
    std::unique_ptr<VulkanPresentCommand> m_presentCommand;
};

}

#endif // DUK_RENDERER_VULKAN_SWAPCHAIN_H

