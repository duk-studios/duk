//
// Created by Ricardo on 04/02/2023.
//

#include <duk_renderer/vulkan/vulkan_renderer.h>

#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/window_win_32.h>
#endif

namespace duk::renderer {

namespace detail {

static std::vector<const char*> s_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

static std::vector<const char*> query_instance_extensions(bool hasValidationLayers) {
    std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};
    if (hasValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#if DUK_PLATFORM_IS_WINDOWS
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif DUK_PLATFORM_IS_LINUX
#endif
    return extensions;
}

static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
                               VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
                               const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
                               void*                                            pUserData) {
    auto debugMessanger = (VulkanDebugMessenger*)pUserData;
    debugMessanger->log(messageSeverity, messageTypes, pCallbackData);
    return VK_FALSE;
}

static std::vector<const char*> query_device_extensions() {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

}

VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) :
    m_instance(VK_NULL_HANDLE),
    m_physicalDevice(VK_NULL_HANDLE),
    m_surface(VK_NULL_HANDLE),
    m_device(VK_NULL_HANDLE) {

    create_vk_instance(vulkanRendererCreateInfo);
    if (vulkanRendererCreateInfo.rendererCreateInfo.window){
        create_vk_surface(vulkanRendererCreateInfo);
    }
    select_vk_physical_device(vulkanRendererCreateInfo.rendererCreateInfo.deviceIndex);
    create_vk_device(vulkanRendererCreateInfo);
    create_vk_swapchain(vulkanRendererCreateInfo);
}

VulkanRenderer::~VulkanRenderer() {
    m_swapchain.reset();
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

void VulkanRenderer::begin_frame() {

}

void VulkanRenderer::end_frame() {

}

ExpectedCommandQueue VulkanRenderer::create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) {

    auto familyIndex = m_queueFamilyIndices[commandQueueCreateInfo.type];
    if (familyIndex == ~0){
        return tl::unexpected<RendererError>(RendererError::INVALID_ARGUMENT, "queue type is not supported by the renderer");
    }

    VulkanCommandQueueCreateInfo vulkanCommandQueueCreateInfo = {};
    vulkanCommandQueueCreateInfo.device = m_device;
    vulkanCommandQueueCreateInfo.familyIndex = familyIndex;
    vulkanCommandQueueCreateInfo.index = 0;

    return std::make_shared<VulkanCommandQueue>(vulkanCommandQueueCreateInfo);
}

void VulkanRenderer::create_vk_instance(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) {
    auto& rendererCreateInfo = vulkanRendererCreateInfo.rendererCreateInfo;

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = rendererCreateInfo.applicationName;
    applicationInfo.applicationVersion = rendererCreateInfo.applicationVersion;
    applicationInfo.pEngineName = rendererCreateInfo.engineName;
    applicationInfo.engineVersion = rendererCreateInfo.engineVersion;
    applicationInfo.apiVersion = VK_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    auto extensions = detail::query_instance_extensions(vulkanRendererCreateInfo.hasValidationLayers);
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (vulkanRendererCreateInfo.hasValidationLayers) {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(detail::s_validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = detail::s_validationLayers.data();

        debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = detail::debug_callback;
        debugCreateInfo.pUserData = &m_debugMessenger;
        m_debugMessenger.enabledMessageTypes = debugCreateInfo.messageType;
        m_debugMessenger.enabledMessageSeverity = debugCreateInfo.messageSeverity;
        m_debugMessenger.logger = rendererCreateInfo.logger;

        instanceCreateInfo.pNext = &debugCreateInfo;
    }
    else {
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;
    }

    auto result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS){
        throw std::runtime_error("failed to create VkInstance");
    }
}

void VulkanRenderer::select_vk_physical_device(uint32_t deviceIndex) {

    VulkanPhysicalDeviceCreateInfo physicalDeviceCreateInfo = {};
    physicalDeviceCreateInfo.instance = m_instance;
    physicalDeviceCreateInfo.deviceIndex = deviceIndex;

    m_physicalDevice = std::make_unique<VulkanPhysicalDevice>(physicalDeviceCreateInfo);
}

void VulkanRenderer::create_vk_surface(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) {
    auto window = vulkanRendererCreateInfo.rendererCreateInfo.window;
    assert(window);

#if DUK_PLATFORM_IS_WINDOWS

    auto windowWin32 = dynamic_cast<platform::WindowWin32*>(window);
    assert(windowWin32);

    VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
    win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32SurfaceCreateInfo.hwnd = windowWin32->win32_window_handle();
    win32SurfaceCreateInfo.hinstance = windowWin32->win32_instance_handle();

    auto result = vkCreateWin32SurfaceKHR(m_instance, &win32SurfaceCreateInfo, nullptr, &m_surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkSurfaceKHR with vkCreateWin32SurfaceKHR");
    }
#endif
}

void VulkanRenderer::create_vk_device(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) {

    m_queueFamilyIndices.fill(~0);
    std::set<uint32_t> uniqueQueueFamilies;

    {
        auto expectedGraphicsQueueProperties = m_physicalDevice->find_queue_family(m_surface, VK_QUEUE_GRAPHICS_BIT);
        if (!expectedGraphicsQueueProperties){
            throw std::runtime_error("could not find a suitable graphics queue");
        }

        auto graphicsQueueProperties = expectedGraphicsQueueProperties.value();

        m_queueFamilyIndices[CommandQueueType::QUEUE_GRAPHICS] = graphicsQueueProperties.familyIndex;
        uniqueQueueFamilies.insert(graphicsQueueProperties.familyIndex);
    }

    {
        // this is terrible, someday I should bother making this better
        auto expectedComputeQueueProperties = m_physicalDevice->find_queue_family(VK_NULL_HANDLE, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
        if (expectedComputeQueueProperties){
            auto computeQueueProperties = expectedComputeQueueProperties.value();
            m_queueFamilyIndices[CommandQueueType::QUEUE_COMPUTE] = computeQueueProperties.familyIndex;
            uniqueQueueFamilies.insert(computeQueueProperties.familyIndex);
        }
        else {
            expectedComputeQueueProperties = m_physicalDevice->find_queue_family(VK_NULL_HANDLE, VK_QUEUE_COMPUTE_BIT);
            if (expectedComputeQueueProperties){
                auto computeQueueProperties = expectedComputeQueueProperties.value();
                m_queueFamilyIndices[CommandQueueType::QUEUE_COMPUTE] = computeQueueProperties.familyIndex;
                uniqueQueueFamilies.insert(computeQueueProperties.familyIndex);
            }
        }
    }

    std::vector<uint32_t> queueFamilyIndices(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    for (auto familyIndex : queueFamilyIndices) {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
        graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = familyIndex;
        graphicsQueueCreateInfo.queueCount = 1;
        graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(graphicsQueueCreateInfo);
    }


    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();

    auto deviceExtensions = detail::query_device_extensions();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Note: this is ignored on modern vulkan implementations,
    // but it's good to keep here for compatibility with older versions
    // (you never know when you're going to need it)
    if (vulkanRendererCreateInfo.hasValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(detail::s_validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = detail::s_validationLayers.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    auto result = vkCreateDevice(m_physicalDevice->handle(), &deviceCreateInfo, nullptr, &m_device);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkDevice");
    }
}

void VulkanRenderer::create_vk_swapchain(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) {
    VulkanSwapchainCreateInfo swapchainCreateInfo = {};
    swapchainCreateInfo.physicalDevice = m_physicalDevice.get();
    swapchainCreateInfo.device = m_device;
    swapchainCreateInfo.surface = m_surface;
    swapchainCreateInfo.sharedQueueFamilyIndices = {m_queueFamilyIndices[CommandQueueType::QUEUE_GRAPHICS]};
    auto window = vulkanRendererCreateInfo.rendererCreateInfo.window;
    swapchainCreateInfo.extent = {window->width(), window->height()};


    m_swapchain = std::make_unique<VulkanSwapchain>(swapchainCreateInfo);
}

}