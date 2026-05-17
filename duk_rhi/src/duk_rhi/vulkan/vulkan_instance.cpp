//
// Created by Ricardo on 04/02/2023.
//

#include <duk_rhi/exception.h>
#include <duk_rhi/vulkan/vulkan_instance.h>
#include <duk_rhi/vulkan/vulkan_command_context.h>
#include <duk_rhi/vulkan/vulkan_image.h>

#include <duk_log/log.h>

#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/window_win_32.h>
#elif DUK_PLATFORM_IS_LINUX
#include <duk_platform/linux/window_xcb.h>
#endif

#include <unordered_map>

namespace duk::rhi {

namespace detail {
static std::vector<const char*> s_validationLayers = {"VK_LAYER_KHRONOS_validation"};

static bool has_validation_layers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName: s_validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}

static std::vector<const char*> query_instance_extensions(bool hasValidationLayers) {
    std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};
    if (hasValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#if DUK_PLATFORM_IS_WINDOWS
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif DUK_PLATFORM_IS_LINUX
    extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
    return extensions;
}

duk::log::Level log_level_from_severity(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return log::Level::VERBOSE;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return log::Level::INFO;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return log::Level::WARN;
        default:
            return log::Level::FATAL;
    }
}

static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    auto logger = (duk::log::Logger*)pUserData;
    if (!logger) {
        return VK_FALSE;
    }
    logger->print(detail::log_level_from_severity(messageSeverity), "Vulkan validation: {}", std::string(pCallbackData->pMessage));
    return VK_FALSE;
}

static std::vector<const char*> query_device_extensions() {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME};
}

struct ResolvedQueue {
    uint32_t familyIndex;
    uint32_t queueIndex;
    VkQueueFlags flags;
};

struct ResolvedQueues {
    std::vector<VkDeviceQueueCreateInfo> createInfos;
    std::vector<std::vector<float>> priorities;
    std::vector<ResolvedQueue> queues;
};

static ResolvedQueues resolve_queues(const VulkanPhysicalDevice* physicalDevice) {
    const auto& familyProps = physicalDevice->queue_family_properties();

    ResolvedQueues result;

    for (uint32_t familyIndex = 0; familyIndex < static_cast<uint32_t>(familyProps.size()); ++familyIndex) {
        const auto& fp = familyProps[familyIndex];

        result.priorities.emplace_back(fp.queueCount, 1.0f);

        VkDeviceQueueCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = familyIndex;
        info.queueCount = fp.queueCount;
        info.pQueuePriorities = result.priorities.back().data();
        result.createInfos.push_back(info);

        for (uint32_t queueIndex = 0; queueIndex < fp.queueCount; ++queueIndex) {
            result.queues.push_back({familyIndex, queueIndex, fp.queueFlags});
        }
    }

    return result;
}

static VkQueueFlags command_queue_type_to_vk_flags(CommandQueue::Type::Mask type) {
    VkQueueFlags flags = 0;
    if (type & CommandQueue::Type::GRAPHICS) {
        flags |= VK_QUEUE_GRAPHICS_BIT;
    }
    if (type & CommandQueue::Type::COMPUTE) {
        flags |= VK_QUEUE_COMPUTE_BIT;
    }
    if (type & CommandQueue::Type::TRANSFER) {
        flags |= VK_QUEUE_TRANSFER_BIT;
    }
    return flags;
}

static bool queue_supports_present(VkPhysicalDevice physicalDevice, uint32_t familyIndex, const platform::Window* window) {
    if (!window) {
        return true;
    }
#if DUK_PLATFORM_IS_WINDOWS
    return vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, familyIndex) == VK_TRUE;
#elif DUK_PLATFORM_IS_LINUX
    // TODO: obtain XCB connection and visual_id from the window to call
    //       vkGetPhysicalDeviceXcbPresentationSupportKHR when that path is needed.
    return true;
#else
    return false;
#endif
}

static VulkanQueue* find_queue(VkPhysicalDevice physicalDevice, const std::vector<std::shared_ptr<VulkanQueue>>& queues, CommandQueue::Type::Mask type, const platform::Window* window) {
    auto requiredFlags = command_queue_type_to_vk_flags(type);

    for (const auto& queue : queues) {
        if ((queue->flags() & requiredFlags) != requiredFlags) {
            continue;
        }
        if (!queue_supports_present(physicalDevice, queue->family_index(), window)) {
            continue;
        }
        return queue.get();
    }

    return nullptr;
}

static std::vector<std::shared_ptr<VulkanQueue>> create_vulkan_queues(VulkanInstance& instance, const ResolvedQueues& resolved) {
    std::vector<std::shared_ptr<VulkanQueue>> queues;

    for (const auto& r : resolved.queues) {
        VulkanQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.instance = &instance;
        queueCreateInfo.flags = r.flags;
        queueCreateInfo.familyIndex = r.familyIndex;
        queueCreateInfo.queueIndex = r.queueIndex;
        queues.push_back(std::make_shared<VulkanQueue>(queueCreateInfo));
    }

    return queues;
}

static VkDevice create_vk_device(VulkanPhysicalDevice* physicalDevice, const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, bool hasValidationLayers) {
    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timelineSemaphoreFeatures = {};
    timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;
    timelineSemaphoreFeatures.timelineSemaphore = VK_TRUE;

    VkPhysicalDeviceRobustness2FeaturesEXT physicalDeviceRobustness2 = {};
    physicalDeviceRobustness2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
    physicalDeviceRobustness2.nullDescriptor = VK_TRUE;
    physicalDeviceRobustness2.pNext = &timelineSemaphoreFeatures;

    const auto& deviceFeatures = physicalDevice->features();

    VkPhysicalDeviceFeatures2 enabledFeatures = {};
    enabledFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    enabledFeatures.features.fillModeNonSolid = VK_TRUE;
    enabledFeatures.features.robustBufferAccess = VK_TRUE;
    if (deviceFeatures.multiDrawIndirect) {
        enabledFeatures.features.multiDrawIndirect = VK_TRUE;
    }
    enabledFeatures.pNext = &physicalDeviceRobustness2;

    auto deviceExtensions = query_device_extensions();

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pNext = &enabledFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (hasValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = s_validationLayers.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    VkDevice device = VK_NULL_HANDLE;
    auto result = vkCreateDevice(physicalDevice->handle(), &deviceCreateInfo, nullptr, &device);
    volkLoadDevice(device);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkDevice");
    }

    return device;
}

static Capabilities populate_capabilities(VulkanPhysicalDevice* physicalDevice) {
    Capabilities caps = {};
    // Multi-draw indirect support.
    caps.multiDrawIndirectSupported = physicalDevice->features().multiDrawIndirect;

    // Buffer alignment requirements.
    const auto& limits = physicalDevice->properties().limits;
    caps.minUniformBufferOffsetAlignment = static_cast<size_t>(limits.minUniformBufferOffsetAlignment);
    caps.minStorageBufferOffsetAlignment = static_cast<size_t>(limits.minStorageBufferOffsetAlignment);

    return caps;
}
} // namespace detail

VulkanInstance::VulkanInstance(const VulkanRHICreateInfo& createInfo)
    : m_instance(VK_NULL_HANDLE)
    , m_debugMessenger(VK_NULL_HANDLE)
    , m_device(VK_NULL_HANDLE) {
    volkInitialize();

    create_vk_instance(createInfo);
    select_vk_physical_device(createInfo.rhiCreateInfo.deviceIndex);

    auto resolved = detail::resolve_queues(m_physicalDevice.get());
    m_device = detail::create_vk_device(m_physicalDevice.get(), resolved.createInfos, createInfo.hasValidationLayers);
    m_queues = detail::create_vulkan_queues(*this, resolved);
}

VulkanInstance::~VulkanInstance() {
    vkDeviceWaitIdle(m_device);
    vkDestroyDevice(m_device, nullptr);
    if (m_debugMessenger) {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }
    vkDestroyInstance(m_instance, nullptr);
    volkFinalize();
}

const Capabilities& VulkanInstance::capabilities() const {
    return m_capabilities;
}

std::unique_ptr<CommandContext> VulkanInstance::create_command_context(const CommandContextCreateInfo& commandContextCreateInfo) {
    auto* queue = detail::find_queue(m_physicalDevice->handle(), m_queues, commandContextCreateInfo.type, commandContextCreateInfo.window);
    if (!queue) {
        throw std::runtime_error("failed to find a VulkanQueue matching the requested capabilities");
    }

    std::unique_ptr<VulkanSwapchain> swapchain;
    if (commandContextCreateInfo.window) {
        VulkanSwapchainCreateInfo swapchainCreateInfo = {};
        swapchainCreateInfo.instance = m_instance;
        swapchainCreateInfo.device = m_device;
        swapchainCreateInfo.physicalDevice = m_physicalDevice.get();
        swapchainCreateInfo.window = commandContextCreateInfo.window;
        swapchainCreateInfo.framesInFlight = commandContextCreateInfo.framesInFlight;
        swapchain = std::make_unique<VulkanSwapchain>(swapchainCreateInfo);
    }

    VulkanCommandContextCreateInfo vulkanCommandContextInfo = {};
    vulkanCommandContextInfo.instance = this;
    vulkanCommandContextInfo.physicalDevice = m_physicalDevice.get();
    vulkanCommandContextInfo.queue = queue;
    vulkanCommandContextInfo.framesInFlight = commandContextCreateInfo.framesInFlight;
    return std::make_unique<VulkanCommandContext>(vulkanCommandContextInfo, std::move(swapchain));
}

std::unique_ptr<CommandQueue> VulkanInstance::create_command_queue(const CommandContextCreateInfo& commandContextCreateInfo) {
    return std::make_unique<CommandQueue>(create_command_context(commandContextCreateInfo));
}

std::unique_lock<std::shared_mutex> VulkanInstance::unique_device_lock() {
    return std::unique_lock(m_deviceMutex);
}

std::shared_lock<std::shared_mutex> VulkanInstance::shared_device_lock() {
    return std::shared_lock(m_deviceMutex);
}

VkResult VulkanInstance::wait_idle() const {
    return vkDeviceWaitIdle(m_device);
}

VkInstance VulkanInstance::handle() const {
    return m_instance;
}

VkDevice VulkanInstance::device() const {
    return m_device;
}

void VulkanInstance::create_vk_instance(const VulkanRHICreateInfo& createInfo) {
    auto& rhiInfo = createInfo.rhiCreateInfo;

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = rhiInfo.applicationName;
    applicationInfo.applicationVersion = rhiInfo.applicationVersion;
    applicationInfo.pEngineName = rhiInfo.engineName;
    applicationInfo.engineVersion = VK_MAKE_VERSION(rhiInfo.engineVersion, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_1;

    bool hasValidationLayers = createInfo.hasValidationLayers && detail::has_validation_layers();

    auto extensions = detail::query_instance_extensions(hasValidationLayers);

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (hasValidationLayers) {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(detail::s_validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = detail::s_validationLayers.data();

        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = detail::debug_callback;
        debugCreateInfo.pUserData = rhiInfo.logger;
        instanceCreateInfo.pNext = &debugCreateInfo;
    } else {
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;
    }

    auto result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkInstance");
    }

    volkLoadInstanceOnly(m_instance);

    if (hasValidationLayers) {
        result = vkCreateDebugUtilsMessengerEXT(m_instance, &debugCreateInfo, nullptr, &m_debugMessenger);
        if (result != VK_SUCCESS && rhiInfo.logger) {
            duk::log::warn(rhiInfo.logger, "failed to create vulkan validation layer debug messenger");
        }
    }
}

void VulkanInstance::select_vk_physical_device(uint32_t deviceIndex) {
    VulkanPhysicalDeviceCreateInfo physicalDeviceCreateInfo = {};
    physicalDeviceCreateInfo.instance = m_instance;
    physicalDeviceCreateInfo.deviceIndex = deviceIndex;

    m_physicalDevice = std::make_unique<VulkanPhysicalDevice>(physicalDeviceCreateInfo);
    m_capabilities = detail::populate_capabilities(m_physicalDevice.get());
}

}// namespace duk::rhi

