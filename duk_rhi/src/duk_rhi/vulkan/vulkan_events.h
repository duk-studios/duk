/// 15/05/2023
/// vulkan_events.h

#ifndef DUK_RHI_VULKAN_EVENTS_H
#define DUK_RHI_VULKAN_EVENTS_H

#include <duk_rhi/vulkan/vulkan_import.h>

#include <duk_event/event.h>

namespace duk::rhi {

using VulkanPrepareFrameEvent = duk::event::EventT<uint32_t>;

struct VulkanSwapchainCreateEventInfo {
    VkFormat format;
    VkExtent2D extent;
    uint32_t imageCount;
    VkSwapchainKHR swapchain;
};

using VulkanSwapchainCreateEvent = duk::event::EventT<VulkanSwapchainCreateEventInfo>;
using VulkanSwapchainCleanEvent = duk::event::EventT<>;

using VulkanResourceCreateImageEvent = duk::event::EventT<uint32_t>;
using VulkanResourceCleanImageEvent = duk::event::EventT<>;

using VulkanResourceCreateFrameBufferEvent = duk::event::EventT<uint32_t>;
using VulkanResourceCleanFrameBufferEvent = duk::event::EventT<>;

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_EVENTS_H
