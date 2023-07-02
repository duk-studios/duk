//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RHI_VULKANDEBUGMESSENGER_H
#define DUK_RHI_VULKANDEBUGMESSENGER_H

#include <duk_rhi/vulkan/vulkan_import.h>

#include <duk_log/logger.h>

namespace duk::rhi {

struct VulkanDebugMessenger {
public:
    VulkanDebugMessenger();

    void log(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) const;

    log::Logger* logger;
    VkDebugUtilsMessageSeverityFlagsEXT enabledMessageSeverity;
    VkDebugUtilsMessageTypeFlagsEXT enabledMessageTypes;
};

} // duk::rhi

#endif //DUK_RHI_VULKANDEBUGMESSENGER_H
