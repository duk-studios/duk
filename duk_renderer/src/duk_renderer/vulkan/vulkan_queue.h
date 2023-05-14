//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_RENDERER_VULKAN_QUEUE_H
#define DUK_RENDERER_VULKAN_QUEUE_H

#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/macros.h>

#include <tl/expected.hpp>

namespace duk::renderer {

class VulkanQueue {
public:

    struct Indices {
        uint32_t index;
        uint32_t familyIndex;
    };

    static constexpr uint32_t invalidQueueFamilyIndex = ~0;

public:

    DUK_NO_DISCARD uint32_t index() const;

    DUK_NO_DISCARD uint32_t family_index() const;


private:
    Indices m_indices;
    VkQueue m_queue;
};

}

#endif //DUK_RENDERER_VULKAN_QUEUE_H
