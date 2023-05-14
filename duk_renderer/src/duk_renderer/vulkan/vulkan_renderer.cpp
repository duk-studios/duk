//
// Created by Ricardo on 04/02/2023.
//

#include <duk_renderer/vulkan/vulkan_renderer.h>

namespace duk::renderer {

namespace detail {

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

    return VK_FALSE;
}

static std::vector<const char*> query_device_extensions() {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

}

VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) :
    m_instance(VK_NULL_HANDLE),
    m_physicalDevice(VK_NULL_HANDLE),
    m_device(VK_NULL_HANDLE) {

    create_vk_instance(vulkanRendererCreateInfo);
    select_physical_device(m_instance, vulkanRendererCreateInfo.rendererCreateInfo.deviceIndex);
    create_vk_device(vulkanRendererCreateInfo);
}

VulkanRenderer::~VulkanRenderer() {
    vkDestroyDevice(m_device, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

void VulkanRenderer::begin_frame() {

}

void VulkanRenderer::end_frame() {

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
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(vulkanRendererCreateInfo.validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = vulkanRendererCreateInfo.validationLayers.data();

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

void VulkanRenderer::select_physical_device(VkInstance instance, uint32_t deviceIndex) {

    VulkanPhysicalDeviceCreateInfo physicalDeviceCreateInfo = {};
    physicalDeviceCreateInfo.instance = instance;
    physicalDeviceCreateInfo.deviceIndex = deviceIndex;

    m_physicalDevice = std::make_unique<VulkanPhysicalDevice>(physicalDeviceCreateInfo);
}

void VulkanRenderer::create_vk_device(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) {

    auto expectedGraphicsQueueProperties = m_physicalDevice->query_queue_family_properties(VK_NULL_HANDLE, VK_QUEUE_GRAPHICS_BIT);

    if (!expectedGraphicsQueueProperties){
        throw std::runtime_error("could not find a suitable graphics queue");
    }

    auto graphicsQueueProperties = expectedGraphicsQueueProperties.value();

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueProperties.familyIndex;
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &graphicsQueueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;

    auto deviceExtensions = detail::query_device_extensions();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Note: this is ignored on modern vulkan implementations,
    // but it's good to keep here for compatibility with older versions
    // (you never know when you're going to need it)
    if (vulkanRendererCreateInfo.hasValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(vulkanRendererCreateInfo.validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = vulkanRendererCreateInfo.validationLayers.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    auto result = vkCreateDevice(m_physicalDevice->handle(), &deviceCreateInfo, nullptr, &m_device);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkDevice");
    }
}

}