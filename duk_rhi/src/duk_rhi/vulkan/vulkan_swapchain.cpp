/// 20/04/2023
/// vulkan_swapchain.cpp

#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>

#include <duk_platform/window.h>

#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/window_win_32.h>
#elif DUK_PLATFORM_IS_LINUX
#include <duk_platform/linux/window_xcb.h>
#endif

#include <duk_rhi/rhi_exception.h>

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
    , m_presentQueue(swapchainCreateInfo.presentQueue)
    , m_framesInFlight(swapchainCreateInfo.framesInFlight) {
    create_platform_surface();
    create_frame_sync();
    create();

    m_listener.listen(m_window->window_resize_event, [this](auto, auto) {
        m_requiresRecreation = true;
    });
}

VulkanSwapchain::~VulkanSwapchain() {
    clean();
    destroy_frame_sync();
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
        throw RHIException(RHIException::INTERNAL_ERROR, "Could not query surface details for swapchain");
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

    VulkanSwapchainImageCreateInfo swapchainImageCreateInfo = {};
    swapchainImageCreateInfo.device = m_device;
    swapchainImageCreateInfo.format = m_surfaceFormat.format;
    swapchainImageCreateInfo.width = m_extent.width;
    swapchainImageCreateInfo.height = m_extent.height;
    swapchainImageCreateInfo.swapchain = m_swapchain;

    m_image = std::make_unique<VulkanSwapchainImage>(swapchainImageCreateInfo);

    VulkanSwapchainCreateEventInfo swapchainInfo = {};
    swapchainInfo.format = m_surfaceFormat.format;
    swapchainInfo.extent = m_extent;
    swapchainInfo.imageCount = m_image->image_count();
    swapchainInfo.swapchain = m_swapchain;

    m_swapchainCreateEvent(swapchainInfo);
}

void VulkanSwapchain::clean() {
    m_swapchainCleanEvent();

    m_image.reset();

    if (m_swapchain) {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

void VulkanSwapchain::recreate() {
    vkDeviceWaitIdle(m_device);
    clean();
    create();
}

uint32_t VulkanSwapchain::acquire_next_image(uint32_t frameIndex) {
    auto& sync = m_frameSync[frameIndex];


    if (m_requiresRecreation && !m_window->minimized()) {
        vkDeviceWaitIdle(m_device);
        recreate();
        m_requiresRecreation = false;
    }

    auto result = vkAcquireNextImageKHR(
            m_device,
            m_swapchain,
            std::numeric_limits<uint64_t>::max(),
            sync.imageAvailableSemaphore,
            VK_NULL_HANDLE,
            &m_currentImage);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        m_requiresRecreation = true;
        m_ableToPresent = false;
        return m_currentImage;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swapchain image");
    }

    m_ableToPresent = true;
    return m_currentImage;
}

void VulkanSwapchain::present(uint32_t imageIndex, uint32_t frameIndex) {
    if (!m_ableToPresent) {
        return;
    }

    auto& sync = m_frameSync[frameIndex];

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &sync.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    auto swapchainHandle = m_swapchain;
    presentInfo.pSwapchains = &swapchainHandle;
    presentInfo.pImageIndices = &imageIndex;

    auto result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        m_requiresRecreation = true;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swapchain image");
    }
}

VulkanFrameSync& VulkanSwapchain::frame_sync(uint32_t frameIndex) {
    return m_frameSync[frameIndex];
}

void VulkanSwapchain::create_platform_surface() {
    DUK_ASSERT(m_window);
#if DUK_PLATFORM_IS_WINDOWS
    auto windowWin32 = dynamic_cast<platform::WindowWin32*>(m_window);
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
    auto windowXCB = dynamic_cast<platform::WindowXCB*>(m_window);
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

void VulkanSwapchain::create_frame_sync() {
    m_frameSync.resize(m_framesInFlight);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (auto& sync: m_frameSync) {
        vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &sync.imageAvailableSemaphore);
        vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &sync.renderFinishedSemaphore);
    }
}

void VulkanSwapchain::destroy_frame_sync() {
    for (auto& sync: m_frameSync) {
        vkDestroySemaphore(m_device, sync.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_device, sync.renderFinishedSemaphore, nullptr);
    }
    m_frameSync.clear();
}

const uint32_t* VulkanSwapchain::current_image_ptr() const {
    return &m_currentImage;
}

uint32_t VulkanSwapchain::image_count() const {
    return m_image->image_count();
}

VkExtent2D VulkanSwapchain::extent() const {
    return m_extent;
}

VkSwapchainKHR VulkanSwapchain::handle() const {
    return m_swapchain;
}

Image* VulkanSwapchain::image() const {
    return m_image.get();
}

VkSurfaceKHR VulkanSwapchain::vk_surface() const {
    return m_surface;
}

VulkanSwapchainCreateEvent* VulkanSwapchain::create_event() {
    return &m_swapchainCreateEvent;
}

VulkanSwapchainCleanEvent* VulkanSwapchain::clean_event() {
    return &m_swapchainCleanEvent;
}

}// namespace duk::rhi
