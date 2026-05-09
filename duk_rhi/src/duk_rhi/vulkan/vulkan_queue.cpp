//
// Created by Ricardo on 03/05/2026.
//

#include <duk_rhi/vulkan/vulkan_queue.h>

namespace duk::rhi {

VulkanQueue::VulkanQueue(const VulkanQueueCreateInfo& createInfo)
    : m_handle(VK_NULL_HANDLE)
    , m_flags(createInfo.flags)
    , m_familyIndex(createInfo.familyIndex)
    , m_queueIndex(createInfo.queueIndex)
    , m_mutex(createInfo.mutex) {
    vkGetDeviceQueue(createInfo.device, createInfo.familyIndex, createInfo.queueIndex, &m_handle);
}

VkResult VulkanQueue::submit(uint32_t submitCount, const VkSubmitInfo* submitInfos, VkFence fence) {
    std::lock_guard<std::mutex> lock(*m_mutex);
    return vkQueueSubmit(m_handle, submitCount, submitInfos, fence);
}

VkQueue VulkanQueue::handle() const {
    return m_handle;
}

uint32_t VulkanQueue::family_index() const {
    return m_familyIndex;
}

uint32_t VulkanQueue::queue_index() const {
    return m_queueIndex;
}

VkQueueFlags VulkanQueue::flags() const {
    return m_flags;
}

}// namespace duk::rhi

