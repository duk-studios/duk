//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RENDERER_VULKANDEBUGMESSENGER_H
#define DUK_RENDERER_VULKANDEBUGMESSENGER_H

#include <duk_renderer/vulkan/vulkan_import.h>

#include <duk_log/logger.h>

namespace duk::renderer {

struct VulkanDebugMessenger {
public:
    VulkanDebugMessenger();

    void log(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) const;

    log::Logger* logger;
    VkDebugUtilsMessageSeverityFlagsEXT enabledMessageSeverity;
    VkDebugUtilsMessageTypeFlagsEXT enabledMessageTypes;
};

} // duk::renderer

#endif //DUK_RENDERER_VULKANDEBUGMESSENGER_H
