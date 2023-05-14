//
// Created by Ricardo on 13/04/2023.
//

#include <duk_renderer/vulkan/vulkan_queue.h>

#include <vector>

namespace duk::renderer {

uint32_t VulkanQueue::index() const {
    return m_indices.index;
}

uint32_t VulkanQueue::family_index() const {
    return m_indices.familyIndex;
}

}