/// 20/04/2023
/// vulkan_swapchain.cpp

#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>
#include <duk_rhi/vulkan/vulkan_queue.h>

#include <duk_platform/window.h>

#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/window_win_32.h>
#elif DUK_PLATFORM_IS_LINUX
#include <duk_platform/linux/window_xcb.h>
#endif

#include <duk_rhi/exception.h>

#include <duk_macros/macros.h>

#include <limits>
#include <stdexcept>

namespace duk::rhi {

namespace detail {

VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
    if (formats.size() == 1 && formats.front().format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& availableFormat: formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return formats.front();
}

VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& presentModes) {
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto availablePresentMode: presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
        if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = availablePresentMode;
        }
    }
    return bestMode;
}

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& windowExtent) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = {};
    actualExtent.width = std::clamp<uint32_t>(windowExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp<uint32_t>(windowExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return actualExtent;
}

}// namespace detail

VulkanSwapchain::VulkanSwapchain(const VulkanSwapchainCreateInfo& swapchainCreateInfo)
    : m_instance(swapchainCreateInfo.instance)
    , m_device(swapchainCreateInfo.device)
    , m_physicalDevice(swapchainCreateInfo.physicalDevice)
    , m_window(swapchainCreateInfo.window)
    , m_framesInFlight(swapchainCreateInfo.framesInFlight) {
    create_surface();
    create();
}

VulkanSwapchain::~VulkanSwapchain() {
    clean();
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
}

void VulkanSwapchain::create() {
    if (m_swapchain != VK_NULL_HANDLE) {
        throw std::logic_error("tried to create a swapchain that was already created");
    }

    VulkanSurfaceDetails surfaceDetails = {};
    if (!m_physicalDevice->query_surface_details(m_surface, surfaceDetails)) {
        throw Exception(Exception::INTERNAL_ERROR, "Could not query surface details for swapchain");
    }

    m_surfaceFormat = detail::choose_swap_surface_format(surfaceDetails.formats);

    m_presentMode = detail::choose_swap_present_mode(surfaceDetails.presentModes);

    m_extent = detail::choose_swap_extent(surfaceDetails.capabilities, {m_window->width(), m_window->height()});

    auto imageCount = surfaceDetails.capabilities.minImageCount + 1;
    if (surfaceDetails.capabilities.maxImageCount > 0 && imageCount > surfaceDetails.capabilities.maxImageCount) {
        imageCount = surfaceDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = m_surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = m_surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = m_surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = m_extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = surfaceDetails.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = m_presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    auto result = vkCreateSwapchainKHR(m_device, &swapchainCreateInfo, nullptr, &m_swapchain);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create swapchain");
    }

    // extract images from swapchain
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);

    std::vector<VkImage> swapchainImages;
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, swapchainImages.data());

    m_images.resize(imageCount);

    VulkanExternalImageCreateInfo externalImageCreateInfo = {};
    externalImageCreateInfo.device = m_device;
    externalImageCreateInfo.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    externalImageCreateInfo.format = m_surfaceFormat.format;
    externalImageCreateInfo.width = m_extent.width;
    externalImageCreateInfo.height = m_extent.height;
    for (auto i = 0; i < imageCount; i++) {
        externalImageCreateInfo.image = swapchainImages[i];
        m_images[i] = std::make_unique<VulkanImage>(externalImageCreateInfo);
    }

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    m_imageAcquiredSemaphores.resize(m_framesInFlight);
    for (auto& semaphore : m_imageAcquiredSemaphores) {
        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
            throw std::runtime_error("VulkanSwapchain: failed to create image semaphore");
        }
    }

    m_imageRenderedSemaphores.resize(m_images.size());
    for (auto& semaphore : m_imageRenderedSemaphores) {
        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
            throw std::runtime_error("VulkanSwapchain: failed to create render semaphore");
        }
    }
}

void VulkanSwapchain::clean() {
    m_images.clear();
    if (m_swapchain) {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
    for (auto semaphore : m_imageAcquiredSemaphores) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    m_imageAcquiredSemaphores.clear();
    for (auto semaphore : m_imageRenderedSemaphores) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    m_imageRenderedSemaphores.clear();
}

void VulkanSwapchain::recreate() {
    if (m_window->minimized() || !m_window->valid()) {
        return;
    }
    clean();
    create();
}

VkResult VulkanSwapchain::acquire_next_image(uint32_t frameIndex) {
    uint32_t imageIndex;
    const auto result = vkAcquireNextImageKHR(
            m_device,
            m_swapchain,
            std::numeric_limits<uint64_t>::max(),
            m_imageAcquiredSemaphores[frameIndex],
            VK_NULL_HANDLE,
            &imageIndex);
    if (result == VK_SUCCESS)  {
        m_imageIndex = imageIndex;
    }
    return result;
}

VkResult VulkanSwapchain::present(VulkanQueue& queue) {
    if (!m_imageIndex) {
        throw std::logic_error("tried to present a swapchain image before acquiring one");
    }
    const auto imageIndex = m_imageIndex.value();
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_imageRenderedSemaphores[imageIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &imageIndex;

    m_imageIndex = std::nullopt;

    return queue.present(presentInfo);
}

uint32_t VulkanSwapchain::image_count() const {
    return m_images.size();
}

VkExtent2D VulkanSwapchain::extent() const {
    return m_extent;
}

VulkanImage* VulkanSwapchain::image() const {
    if (m_imageIndex) {
        return m_images.at(m_imageIndex.value()).get();
    }
    return nullptr;
}

VkSemaphore VulkanSwapchain::image_acquired_semaphore(uint32_t frameIndex) const {
    return m_imageAcquiredSemaphores[frameIndex];
}

VkSemaphore VulkanSwapchain::image_rendered_semaphore() const {
    if (m_imageIndex) {
        return m_imageRenderedSemaphores[m_imageIndex.value()];
    }
    return VK_NULL_HANDLE;
}

void VulkanSwapchain::create_surface() {
    DUK_ASSERT(m_window);
#if DUK_PLATFORM_IS_WINDOWS
    auto windowWin32 = dynamic_cast<const platform::WindowWin32*>(m_window);
    DUK_ASSERT(windowWin32);

    VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
    win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32SurfaceCreateInfo.hwnd = windowWin32->win32_window_handle();
    win32SurfaceCreateInfo.hinstance = windowWin32->win32_instance_handle();

    auto result = vkCreateWin32SurfaceKHR(m_instance, &win32SurfaceCreateInfo, nullptr, &m_surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkSurfaceKHR with vkCreateWin32SurfaceKHR");
    }
#elif DUK_PLATFORM_IS_LINUX
    auto windowXCB = dynamic_cast<const platform::WindowXCB*>(m_window);
    DUK_ASSERT(windowXCB);
    VkXcbSurfaceCreateInfoKHR xcbSurfaceCreateInfo = {};
    xcbSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcbSurfaceCreateInfo.connection = windowXCB->xcb_connection();
    xcbSurfaceCreateInfo.window = windowXCB->xcb_window_handle();
    auto result = vkCreateXcbSurfaceKHR(m_instance, &xcbSurfaceCreateInfo, nullptr, &m_surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkSurfaceKHR with vkCreateXcbSurfaceKHR");
    }
#endif
}

}// namespace duk::rhi
