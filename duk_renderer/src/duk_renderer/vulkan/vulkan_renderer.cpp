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
    return extensions;
}

static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
                               VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
                               const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
                               void*                                            pUserData) {

    return VK_FALSE;
}

}


VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) :
    m_instance(VK_NULL_HANDLE),
    m_physicalDevice(VK_NULL_HANDLE),
    m_device(VK_NULL_HANDLE) {

    create_vk_instance(vulkanRendererCreateInfo);
    select_physical_device(vulkanRendererCreateInfo);

}

VulkanRenderer::~VulkanRenderer() {
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

        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
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

void VulkanRenderer::select_physical_device(const VulkanRendererCreateInfo& vulkanRendererCreateInfo) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    auto desiredGPUIndex = vulkanRendererCreateInfo.rendererCreateInfo.gpuIndex;
    if (desiredGPUIndex >= devices.size()){
        throw std::runtime_error("gpu index not found");
    }

    m_physicalDevice = devices[desiredGPUIndex];
}


}