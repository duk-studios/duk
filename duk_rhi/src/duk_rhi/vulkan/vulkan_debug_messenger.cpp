//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/vulkan/vulkan_debug_messenger.h>

namespace duk::rhi {

namespace detail {

log::Level level_from_severity(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
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

}// namespace detail

VulkanDebugMessenger::VulkanDebugMessenger()
    : logger(nullptr)
    , enabledMessageSeverity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    , enabledMessageTypes(0) {
}

void VulkanDebugMessenger::log(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                               VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) const {
    if (!logger) {
        return;
    }

    if ((messageSeverity < enabledMessageSeverity) == 0) {
        return;
    }

    if ((messageTypes & enabledMessageTypes) == 0) {
        return;
    }

    logger->print(detail::level_from_severity(messageSeverity), "Vulkan validation: {}", std::string(pCallbackData->pMessage));
}

}// namespace duk::rhi