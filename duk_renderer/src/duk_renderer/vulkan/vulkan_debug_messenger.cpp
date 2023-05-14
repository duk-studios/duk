//
// Created by Ricardo on 09/04/2023.
//

#include <duk_renderer/vulkan/vulkan_debug_messenger.h>

namespace duk::renderer {

VulkanDebugMessenger::VulkanDebugMessenger() :
    logger(nullptr),
    enabledMessageSeverity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
    enabledMessageTypes(0){

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

    logger->log() << pCallbackData->pMessage;
}

} // duk::renderer