/// 15/05/2023
/// vulkan_events.h

#ifndef DUK_RENDERER_VULKAN_EVENTS_H
#define DUK_RENDERER_VULKAN_EVENTS_H

#include <duk_renderer/vulkan/vulkan_import.h>

#include <duk_events/event.h>

namespace duk::renderer {

using VulkanPrepareFrameEvent = duk::events::EventT<uint32_t>;

struct VulkanSwapchainCreateEventInfo {
    VkFormat format;
    VkExtent2D extent;
    uint32_t imageCount;
    VkSwapchainKHR swapchain;
};

using VulkanSwapchainCreateEvent = duk::events::EventT<VulkanSwapchainCreateEventInfo>;
using VulkanSwapchainCleanEvent = duk::events::EventT<>;

using VulkanResourceCreateImageEvent = duk::events::EventT<uint32_t>;
using VulkanResourceCleanImageEvent = duk::events::EventT<>;

using VulkanResourceCreateFrameBufferEvent = duk::events::EventT<uint32_t>;
using VulkanResourceCleanFrameBufferEvent = duk::events::EventT<>;

}

#endif // DUK_RENDERER_VULKAN_EVENTS_H

