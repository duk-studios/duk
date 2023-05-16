/// 15/05/2023
/// vulkan_events.h

#ifndef DUK_RENDERER_VULKAN_EVENTS_H
#define DUK_RENDERER_VULKAN_EVENTS_H

#include <duk_events/event.h>

namespace duk::renderer {

using VulkanPrepareFrameEvent = duk::events::EventT<uint32_t>;

}

#endif // DUK_RENDERER_VULKAN_EVENTS_H

