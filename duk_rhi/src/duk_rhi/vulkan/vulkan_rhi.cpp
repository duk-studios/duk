//
// Created by Ricardo on 04/02/2023.
//

#include <duk_rhi/rhi_exception.h>
#include <duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_rhi/vulkan/vulkan_rhi.h>

#include <duk_log/log.h>

#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/window_win_32.h>
#endif

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

static uint32_t find_present_queue_index(VulkanPhysicalDevice* physicalDevice, VkSurfaceKHR surface) {
    VulkanQueueFamilyProperties vulkanQueueFamilyProperties = {};

    if (!physicalDevice->find_queue_family(vulkanQueueFamilyProperties, surface, 0)) {
        return ~0;
    }

    auto presentQueueProperties = vulkanQueueFamilyProperties;
    return presentQueueProperties.familyIndex;
}

static uint32_t find_graphics_queue_index(VulkanPhysicalDevice* physicalDevice) {
    VulkanQueueFamilyProperties vulkanQueueFamilyProperties = {};

    if (!physicalDevice->find_queue_family(vulkanQueueFamilyProperties, VK_NULL_HANDLE, VK_QUEUE_GRAPHICS_BIT)) {
        return ~0;
    }

    auto graphicsQueueProperties = vulkanQueueFamilyProperties;
    return graphicsQueueProperties.familyIndex;
}

static uint32_t find_compute_queue_index(VulkanPhysicalDevice* physicalDevice) {
    // try to find an exclusive compute queue
    VulkanQueueFamilyProperties vulkanQueueFamilyProperties = {};

    if (physicalDevice->find_queue_family(vulkanQueueFamilyProperties, VK_NULL_HANDLE, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT)) {
        auto computeQueueProperties = vulkanQueueFamilyProperties;
        return computeQueueProperties.familyIndex;
    }

    // if not found, try to find any queue that supports compute
    if (physicalDevice->find_queue_family(vulkanQueueFamilyProperties, VK_NULL_HANDLE, VK_QUEUE_COMPUTE_BIT)) {
        auto computeQueueProperties = vulkanQueueFamilyProperties;
        return computeQueueProperties.familyIndex;
    }

    return ~0;
}

}// namespace detail

VulkanRHI::VulkanRHI(const VulkanRHICreateInfo& vulkanRendererCreateInfo)
    : m_instance(VK_NULL_HANDLE)
    , m_debugMessenger(VK_NULL_HANDLE)
    , m_physicalDevice(VK_NULL_HANDLE)
    , m_surface(VK_NULL_HANDLE)
    , m_device(VK_NULL_HANDLE)
    , m_maxFramesInFlight(vulkanRendererCreateInfo.maxFramesInFlight)
    , m_currentFrame(0) {
    volkInitialize();

    create_vk_instance(vulkanRendererCreateInfo);
    if (vulkanRendererCreateInfo.renderHardwareInterfaceCreateInfo.window) {
        create_vk_surface(vulkanRendererCreateInfo);
    }
    select_vk_physical_device(vulkanRendererCreateInfo.renderHardwareInterfaceCreateInfo.deviceIndex);
    create_vk_device(vulkanRendererCreateInfo);
    if (m_surface) {
        create_vk_swapchain(vulkanRendererCreateInfo);
    }
    create_resource_manager();
    create_descriptor_set_layout_cache();
    create_sampler_cache();
    create_command_scheduler();
}

VulkanRHI::~VulkanRHI() {
    vkDeviceWaitIdle(m_device);
    m_commandScheduler.reset();
    m_descriptorSetLayoutCache.reset();
    m_samplerCache.reset();
    m_swapchain.reset();
    m_resourceManager.reset();
    vkDestroyDevice(m_device, nullptr);
    if (m_surface) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }
    if (m_debugMessenger) {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }
    vkDestroyInstance(m_instance, nullptr);

    volkFinalize();
}

void VulkanRHI::prepare_frame() {
    m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
    m_commandScheduler->begin();
    m_prepareFrameEvent(m_currentFrame);
}

void VulkanRHI::update() {
    m_resourceManager->update(m_swapchain ? *m_swapchain->current_image_ptr() : m_currentFrame);
}

Command* VulkanRHI::acquire_image_command() {
    if (!m_swapchain) {
        return nullptr;
    }
    return m_swapchain->acquire_image_command();
}

Command* VulkanRHI::present_command() {
    if (!m_swapchain) {
        return nullptr;
    }
    return m_swapchain->present_command();
}

Image* VulkanRHI::present_image() {
    if (!m_swapchain) {
        return nullptr;
    }
    return m_swapchain->image();
}

RHICapabilities* VulkanRHI::capabilities() const {
    return m_rendererCapabilities.get();
}

std::shared_ptr<CommandQueue> VulkanRHI::create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) {
    auto it = m_queueFamilyIndices.find(commandQueueCreateInfo.type);
    if (it == m_queueFamilyIndices.end()) {
        throw RHIException(RHIException::INTERNAL_ERROR, "Failed to create command queue");
    }

    auto queueFamilyIndex = it->second;

    VulkanCommandQueueCreateInfo vulkanCommandQueueCreateInfo = {};
    vulkanCommandQueueCreateInfo.device = m_device;
    vulkanCommandQueueCreateInfo.familyIndex = queueFamilyIndex;
    vulkanCommandQueueCreateInfo.index = 0;
    vulkanCommandQueueCreateInfo.currentFramePtr = &m_currentFrame;
    vulkanCommandQueueCreateInfo.currentImagePtr = m_swapchain ? m_swapchain->current_image_ptr() : nullptr;
    vulkanCommandQueueCreateInfo.frameCount = m_maxFramesInFlight;
    vulkanCommandQueueCreateInfo.imageCount = m_swapchain ? m_swapchain->image_count() : m_maxFramesInFlight;
    vulkanCommandQueueCreateInfo.prepareFrameEvent = &m_prepareFrameEvent;

    return m_resourceManager->create(vulkanCommandQueueCreateInfo);
}

CommandScheduler* VulkanRHI::command_scheduler() {
    return m_commandScheduler.get();
}

std::shared_ptr<Shader> VulkanRHI::create_shader(const RHI::ShaderCreateInfo& shaderCreateInfo) {
    VulkanShaderCreateInfo vulkanShaderCreateInfo = {};
    vulkanShaderCreateInfo.shaderDataSource = shaderCreateInfo.shaderDataSource;
    vulkanShaderCreateInfo.device = m_device;
    vulkanShaderCreateInfo.descriptorSetLayoutCache = m_descriptorSetLayoutCache.get();
    return std::make_shared<VulkanShader>(vulkanShaderCreateInfo);
}

std::shared_ptr<GraphicsPipeline> VulkanRHI::create_graphics_pipeline(const GraphicsPipelineCreateInfo& pipelineCreateInfo) {
    VulkanGraphicsPipelineCreateInfo vulkanPipelineCreateInfo = {};
    vulkanPipelineCreateInfo.device = m_device;
    vulkanPipelineCreateInfo.imageCount = m_swapchain ? m_swapchain->image_count() : m_maxFramesInFlight;
    vulkanPipelineCreateInfo.shader = dynamic_cast<VulkanShader*>(pipelineCreateInfo.shader);
    vulkanPipelineCreateInfo.renderPass = dynamic_cast<VulkanRenderPass*>(pipelineCreateInfo.renderPass);
    vulkanPipelineCreateInfo.viewport = pipelineCreateInfo.viewport;
    vulkanPipelineCreateInfo.scissor = pipelineCreateInfo.scissor;
    vulkanPipelineCreateInfo.blend = pipelineCreateInfo.blend;
    vulkanPipelineCreateInfo.cullModeMask = pipelineCreateInfo.cullModeMask;
    vulkanPipelineCreateInfo.depthTesting = pipelineCreateInfo.depthTesting;
    vulkanPipelineCreateInfo.topology = pipelineCreateInfo.topology;
    vulkanPipelineCreateInfo.fillMode = pipelineCreateInfo.fillMode;
    vulkanPipelineCreateInfo.resourceManager = m_resourceManager.get();
    return m_resourceManager->create(vulkanPipelineCreateInfo);
}

std::shared_ptr<ComputePipeline> VulkanRHI::create_compute_pipeline(const RHI::ComputePipelineCreateInfo& pipelineCreateInfo) {
    VulkanComputePipelineCreateInfo vulkanPipelineCreateInfo = {};
    vulkanPipelineCreateInfo.device = m_device;
    vulkanPipelineCreateInfo.shader = dynamic_cast<VulkanShader*>(pipelineCreateInfo.shader);
    return m_resourceManager->create(vulkanPipelineCreateInfo);
}

std::shared_ptr<RenderPass> VulkanRHI::create_render_pass(const RHI::RenderPassCreateInfo& renderPassCreateInfo) {
    VulkanRenderPassCreateInfo vulkanRenderPassCreateInfo = {};
    vulkanRenderPassCreateInfo.device = m_device;
    vulkanRenderPassCreateInfo.colorAttachments = renderPassCreateInfo.colorAttachments;
    vulkanRenderPassCreateInfo.colorAttachmentCount = renderPassCreateInfo.colorAttachmentCount;
    vulkanRenderPassCreateInfo.depthAttachment = renderPassCreateInfo.depthAttachment;
    return m_resourceManager->create(vulkanRenderPassCreateInfo);
}

std::shared_ptr<Buffer> VulkanRHI::create_buffer(const RHI::BufferCreateInfo& bufferCreateInfo) {
    VulkanBufferCreateInfo vulkanBufferCreateInfo = {};
    vulkanBufferCreateInfo.updateFrequency = bufferCreateInfo.updateFrequency;
    vulkanBufferCreateInfo.type = bufferCreateInfo.type;
    vulkanBufferCreateInfo.elementCount = bufferCreateInfo.elementCount;
    vulkanBufferCreateInfo.elementSize = bufferCreateInfo.elementSize;
    vulkanBufferCreateInfo.device = m_device;
    vulkanBufferCreateInfo.physicalDevice = m_physicalDevice.get();
    vulkanBufferCreateInfo.commandQueue = dynamic_cast<VulkanCommandQueue*>(bufferCreateInfo.commandQueue);
    vulkanBufferCreateInfo.imageCount = m_swapchain ? m_swapchain->image_count() : m_maxFramesInFlight;
    vulkanBufferCreateInfo.resourceManager = m_resourceManager.get();
    return m_resourceManager->create(vulkanBufferCreateInfo);
}

std::shared_ptr<Image> VulkanRHI::create_image(const RHI::ImageCreateInfo& imageCreateInfo) {
    VulkanMemoryImageCreateInfo memoryImageCreateInfo = {};
    memoryImageCreateInfo.device = m_device;
    memoryImageCreateInfo.physicalDevice = m_physicalDevice.get();
    memoryImageCreateInfo.imageCount = m_swapchain ? m_swapchain->image_count() : m_maxFramesInFlight;
    memoryImageCreateInfo.commandQueue = dynamic_cast<VulkanCommandQueue*>(imageCreateInfo.commandQueue);
    memoryImageCreateInfo.imageDataSource = imageCreateInfo.imageDataSource;
    memoryImageCreateInfo.initialLayout = imageCreateInfo.initialLayout;
    memoryImageCreateInfo.usage = imageCreateInfo.usage;
    memoryImageCreateInfo.updateFrequency = imageCreateInfo.updateFrequency;
    memoryImageCreateInfo.dstStages = imageCreateInfo.dstStages;
    memoryImageCreateInfo.resourceManager = m_resourceManager.get();
    return m_resourceManager->create(memoryImageCreateInfo);
}

std::shared_ptr<DescriptorSet> VulkanRHI::create_descriptor_set(const RHI::DescriptorSetCreateInfo& descriptorSetCreateInfo) {
    VulkanDescriptorSetCreateInfo vulkanDescriptorSetCreateInfo = {};
    vulkanDescriptorSetCreateInfo.device = m_device;
    vulkanDescriptorSetCreateInfo.descriptorSetDescription = descriptorSetCreateInfo.description;
    vulkanDescriptorSetCreateInfo.descriptorSetLayoutCache = m_descriptorSetLayoutCache.get();
    vulkanDescriptorSetCreateInfo.samplerCache = m_samplerCache.get();
    vulkanDescriptorSetCreateInfo.imageCount = m_swapchain ? m_swapchain->image_count() : m_maxFramesInFlight;
    vulkanDescriptorSetCreateInfo.resourceManager = m_resourceManager.get();
    return m_resourceManager->create(vulkanDescriptorSetCreateInfo);
}

std::shared_ptr<FrameBuffer> VulkanRHI::create_frame_buffer(const RHI::FrameBufferCreateInfo& frameBufferCreateInfo) {
    VulkanFrameBufferCreateInfo vulkanFrameBufferCreateInfo = {};
    vulkanFrameBufferCreateInfo.device = m_device;
    vulkanFrameBufferCreateInfo.imageCount = m_swapchain ? m_swapchain->image_count() : m_maxFramesInFlight;
    vulkanFrameBufferCreateInfo.renderPass = dynamic_cast<VulkanRenderPass*>(frameBufferCreateInfo.renderPass);
    vulkanFrameBufferCreateInfo.attachments = reinterpret_cast<VulkanImage**>(frameBufferCreateInfo.attachments);
    vulkanFrameBufferCreateInfo.attachmentCount = frameBufferCreateInfo.attachmentCount;
    vulkanFrameBufferCreateInfo.resourceManager = m_resourceManager.get();
    return m_resourceManager->create(vulkanFrameBufferCreateInfo);
}

void VulkanRHI::create_vk_instance(const VulkanRHICreateInfo& vulkanRendererCreateInfo) {
    auto& rendererCreateInfo = vulkanRendererCreateInfo.renderHardwareInterfaceCreateInfo;

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = rendererCreateInfo.applicationName;
    applicationInfo.applicationVersion = rendererCreateInfo.applicationVersion;
    applicationInfo.pEngineName = rendererCreateInfo.engineName;
    applicationInfo.engineVersion = VK_MAKE_VERSION(rendererCreateInfo.engineVersion, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    bool hasValidationLayers = vulkanRendererCreateInfo.hasValidationLayers && detail::has_validation_layers();

    auto extensions = detail::query_instance_extensions(hasValidationLayers);
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
        debugCreateInfo.pUserData = rendererCreateInfo.logger;

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
        if (result != VK_SUCCESS) {
            // non-critical, just log and proceed
            if (rendererCreateInfo.logger) {
                duk::log::warn(rendererCreateInfo.logger, "failed to create vulkan validation layer debug messenger");
            }
        }
    }
}

void VulkanRHI::select_vk_physical_device(uint32_t deviceIndex) {
    VulkanPhysicalDeviceCreateInfo physicalDeviceCreateInfo = {};
    physicalDeviceCreateInfo.instance = m_instance;
    physicalDeviceCreateInfo.deviceIndex = deviceIndex;

    m_physicalDevice = std::make_unique<VulkanPhysicalDevice>(physicalDeviceCreateInfo);

    VulkanRendererCapabilitiesCreateInfo rendererCapabilitiesCreateInfo = {};
    rendererCapabilitiesCreateInfo.physicalDevice = m_physicalDevice.get();

    m_rendererCapabilities = std::make_unique<VulkanRendererCapabilities>(rendererCapabilitiesCreateInfo);
}

void VulkanRHI::create_vk_surface(const VulkanRHICreateInfo& vulkanRendererCreateInfo) {
    auto window = vulkanRendererCreateInfo.renderHardwareInterfaceCreateInfo.window;
    DUK_ASSERT(window);

#if DUK_PLATFORM_IS_WINDOWS

    auto windowWin32 = dynamic_cast<platform::WindowWin32*>(window);
    DUK_ASSERT(windowWin32);

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

void VulkanRHI::create_vk_device(const VulkanRHICreateInfo& vulkanRendererCreateInfo) {
    std::set<uint32_t> uniqueQueueFamilies;

    if (m_surface) {
        auto presentQueueFamilyIndex = detail::find_present_queue_index(m_physicalDevice.get(), m_surface);
        if (presentQueueFamilyIndex == ~0) {
            throw std::runtime_error("failed to find a present queue");
        }
        m_queueFamilyIndices[CommandQueue::Type::PRESENT] = presentQueueFamilyIndex;
        uniqueQueueFamilies.insert(presentQueueFamilyIndex);
    }

    {
        auto graphicsQueueFamilyIndex = detail::find_graphics_queue_index(m_physicalDevice.get());
        if (graphicsQueueFamilyIndex == ~0) {
            throw std::runtime_error("failed to find a graphics queue");
        }
        m_queueFamilyIndices[CommandQueue::Type::GRAPHICS] = graphicsQueueFamilyIndex;
        uniqueQueueFamilies.insert(graphicsQueueFamilyIndex);
    }

    {
        auto computeQueueFamilyIndex = detail::find_compute_queue_index(m_physicalDevice.get());
        if (computeQueueFamilyIndex != ~0) {
            m_queueFamilyIndices[CommandQueue::Type::COMPUTE] = computeQueueFamilyIndex;
            uniqueQueueFamilies.insert(computeQueueFamilyIndex);
        }
    }

    std::vector<uint32_t> queueFamilyIndices(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    for (auto familyIndex: queueFamilyIndices) {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
        graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = familyIndex;
        graphicsQueueCreateInfo.queueCount = 1;
        graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(graphicsQueueCreateInfo);
    }

    VkPhysicalDeviceRobustness2FeaturesEXT physicalDeviceRobustness2 = {};
    physicalDeviceRobustness2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
    physicalDeviceRobustness2.nullDescriptor = VK_TRUE;

    const auto& deviceFeatures = m_physicalDevice->features();

    VkPhysicalDeviceFeatures2 enabledFeatures = {};
    enabledFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    enabledFeatures.features.fillModeNonSolid = VK_TRUE;
    if (deviceFeatures.multiDrawIndirect) {
        enabledFeatures.features.multiDrawIndirect = VK_TRUE;
    }
    enabledFeatures.pNext = &physicalDeviceRobustness2;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pNext = &enabledFeatures;

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

    volkLoadDevice(m_device);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkDevice");
    }
}

void VulkanRHI::create_vk_swapchain(const VulkanRHICreateInfo& vulkanRendererCreateInfo) {
    VkQueue presentQueue;
    vkGetDeviceQueue(m_device, m_queueFamilyIndices.at(CommandQueue::Type::PRESENT), 0, &presentQueue);

    VulkanSwapchainCreateInfo swapchainCreateInfo = {};
    swapchainCreateInfo.physicalDevice = m_physicalDevice.get();
    swapchainCreateInfo.device = m_device;
    swapchainCreateInfo.surface = m_surface;
    auto window = vulkanRendererCreateInfo.renderHardwareInterfaceCreateInfo.window;
    swapchainCreateInfo.window = window;
    swapchainCreateInfo.presentQueue = presentQueue;
    swapchainCreateInfo.prepareFrameEvent = &m_prepareFrameEvent;
    swapchainCreateInfo.frameCount = m_maxFramesInFlight;
    swapchainCreateInfo.currentFramePtr = &m_currentFrame;

    m_swapchain = std::make_unique<VulkanSwapchain>(swapchainCreateInfo);
}

void VulkanRHI::create_resource_manager() {
    VulkanResourceManagerCreateInfo resourceManagerCreateInfo = {};
    resourceManagerCreateInfo.imageCount = m_maxFramesInFlight;
    resourceManagerCreateInfo.prepareFrameEvent = &m_prepareFrameEvent;
    if (m_swapchain) {
        resourceManagerCreateInfo.swapchain = m_swapchain.get();
        resourceManagerCreateInfo.imageCount = m_swapchain->image_count();
    }

    m_resourceManager = std::make_unique<VulkanResourceManager>(resourceManagerCreateInfo);
}

void VulkanRHI::create_descriptor_set_layout_cache() {
    VulkanDescriptorSetLayoutCacheCreateInfo descriptorSetLayoutCacheCreateInfo = {};
    descriptorSetLayoutCacheCreateInfo.device = m_device;

    m_descriptorSetLayoutCache = std::make_unique<VulkanDescriptorSetLayoutCache>(descriptorSetLayoutCacheCreateInfo);
}

void VulkanRHI::create_sampler_cache() {
    VulkanSamplerCacheCreateInfo samplerCacheCreateInfo = {};
    samplerCacheCreateInfo.device = m_device;

    m_samplerCache = std::make_unique<VulkanSamplerCache>(samplerCacheCreateInfo);
}

void VulkanRHI::create_command_scheduler() {
    VulkanCommandSchedulerCreateInfo commandSchedulerCreateInfo = {};
    commandSchedulerCreateInfo.device = m_device;
    commandSchedulerCreateInfo.frameCount = m_maxFramesInFlight;
    commandSchedulerCreateInfo.currentFramePtr = &m_currentFrame;

    m_commandScheduler = std::make_unique<VulkanCommandScheduler>(commandSchedulerCreateInfo);
}

}// namespace duk::rhi