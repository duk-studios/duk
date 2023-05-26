/// 20/04/2023
/// vulkan_swapchain.cpp

#include <duk_renderer/vulkan/vulkan_swapchain.h>
#include <duk_renderer/vulkan/vulkan_render_pass.h>
#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_image.h>

#include <stdexcept>
#include <limits>

namespace duk::renderer {

namespace detail {

VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
    if (formats.size() == 1 && formats.front().format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto &availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return formats.front();
}

VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& presentModes) {
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto availablePresentMode : presentModes) {
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
    actualExtent.width = std::clamp<uint32_t>(windowExtent.width,
                                              capabilities.minImageExtent.width,
                                              capabilities.maxImageExtent.width);

    actualExtent.height = std::clamp<uint32_t>(windowExtent.height,
                                               capabilities.minImageExtent.height,
                                               capabilities.maxImageExtent.height);
    return actualExtent;
}
}

VulkanImageAcquireCommand::VulkanImageAcquireCommand(const VulkanImageAcquireCommandCreateInfo& commandCreateInfo) :
    m_submitter([](const auto&) {}, true, false, commandCreateInfo.frameCount, commandCreateInfo.currentFramePtr, commandCreateInfo.device) {

}

void VulkanImageAcquireCommand::submit(const VulkanCommandParams& commandParams) {
    // do nothing, as the image was already acquired at the start of the frame
}

Submitter* VulkanImageAcquireCommand::submitter_ptr() {
    return &m_submitter;
}

VulkanPresentCommand::VulkanPresentCommand(const VulkanPresentCommandCreateInfo& commandCreateInfo) :
    m_swapchain(commandCreateInfo.swapchain),
    m_currentImagePtr(commandCreateInfo.currentImagePtr),
    m_presentQueue(commandCreateInfo.presentQueue),
    m_submitter([this](const auto& params) { submit(params); }, false, false, 0, nullptr, VK_NULL_HANDLE) {

}

void VulkanPresentCommand::submit(const VulkanCommandParams& commandParams) {

    auto waitDependency = commandParams.waitDependency;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = m_currentImagePtr;
    if (waitDependency) {
        presentInfo.pWaitSemaphores = waitDependency->semaphores;
        presentInfo.waitSemaphoreCount = waitDependency->semaphoreCount;
    }

    auto result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // TODO: recreate swapchain
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swapchain image!");
    }
}

Submitter* VulkanPresentCommand::submitter_ptr() {
    return &m_submitter;
}

VulkanSwapchain::VulkanSwapchain(const VulkanSwapchainCreateInfo& swapchainCreateInfo) :
    m_device(swapchainCreateInfo.device),
    m_physicalDevice(swapchainCreateInfo.physicalDevice),
    m_surface(swapchainCreateInfo.surface),
    m_requestedExtent(swapchainCreateInfo.extent),
    m_presentQueue(swapchainCreateInfo.presentQueue),
    m_swapchain(VK_NULL_HANDLE) {

    m_listener.listen(*swapchainCreateInfo.prepareFrameEvent, [this](uint32_t currentFrame) {

        // we need to signal the semaphore of the acquire image command
        // other commands might want to wait on this semaphore, but we need to get the current image index
        // at the start of the frame, otherwise we will not be able to use the correct resources for this frame
        auto submitter = m_acquireImageCommand->submitter<VulkanSubmitter>();

        auto result = vkAcquireNextImageKHR(m_device, m_swapchain, std::numeric_limits<uint64_t>::max(), *submitter->semaphore(), VK_NULL_HANDLE, &m_currentImage);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // TODO: recreate swapchain
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swapchain image!");
        }
    });

    VulkanImageAcquireCommandCreateInfo imageAcquireCommandCreateInfo = {};
    imageAcquireCommandCreateInfo.device = m_device;
    imageAcquireCommandCreateInfo.frameCount = swapchainCreateInfo.frameCount;
    imageAcquireCommandCreateInfo.currentFramePtr = swapchainCreateInfo.currentFramePtr;

    m_acquireImageCommand = std::make_unique<VulkanImageAcquireCommand>(imageAcquireCommandCreateInfo);

    create();
}

VulkanSwapchain::~VulkanSwapchain() {
    clean();
}

void VulkanSwapchain::create() {
    if (m_swapchain != VK_NULL_HANDLE) {
        throw std::logic_error("tried to create a swapchain that was already created");
    }

    auto surfaceDetails = m_physicalDevice->query_surface_details(m_surface).value();

    m_surfaceFormat = detail::choose_swap_surface_format(surfaceDetails.formats);

    m_presentMode = detail::choose_swap_present_mode(surfaceDetails.presentModes);

    m_extent = detail::choose_swap_extent(surfaceDetails.capabilities, m_requestedExtent);

    //Old code, should check if this is right
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

    //TODO: share images?
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
    swapchainImageCreateInfo.swapchain = m_swapchain;
    swapchainImageCreateInfo.format = m_surfaceFormat.format;

    m_image = std::make_unique<VulkanImage>(swapchainImageCreateInfo);


    VulkanPresentCommandCreateInfo presentCommandCreateInfo = {};
    presentCommandCreateInfo.device = m_device;
    presentCommandCreateInfo.swapchain = m_swapchain;
    presentCommandCreateInfo.currentImagePtr = &m_currentImage;
    presentCommandCreateInfo.presentQueue = m_presentQueue;

    m_presentCommand = std::make_unique<VulkanPresentCommand>(presentCommandCreateInfo);
}

void VulkanSwapchain::clean() {
    m_image.reset();
    if (m_swapchain) {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

VulkanImageAcquireCommand* VulkanSwapchain::acquire_image_command() {


    return m_acquireImageCommand.get();
}

VulkanPresentCommand* VulkanSwapchain::present_command() {
    return m_presentCommand.get();
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

VulkanImage* VulkanSwapchain::image() const {
    return m_image.get();
}

}
