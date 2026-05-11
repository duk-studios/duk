//
// Created by Ricardo on 03/05/2026.
//

#include <duk_rhi/vulkan/vulkan_queue.h>
#include <duk_rhi/vulkan/vulkan_rhi.h>

namespace duk::rhi {

VulkanQueue::VulkanQueue(const VulkanQueueCreateInfo& createInfo)
    : m_instance(*createInfo.instance)
    , m_handle(VK_NULL_HANDLE)
    , m_flags(createInfo.flags)
    , m_familyIndex(createInfo.familyIndex)
    , m_queueIndex(createInfo.queueIndex) {
    vkGetDeviceQueue(m_instance.device(), createInfo.familyIndex, createInfo.queueIndex, &m_handle);
}

VkResult VulkanQueue::submit(uint32_t submitCount, const VkSubmitInfo* submitInfos, VkFence fence) {
    std::unique_lock lock(m_mutex);
    auto deviceLock = m_instance.shared_device_lock();
    return vkQueueSubmit(m_handle, submitCount, submitInfos, fence);
}

VkResult VulkanQueue::present(const VkPresentInfoKHR& presentInfo) {
    std::unique_lock lock(m_mutex);
    auto deviceLock = m_instance.shared_device_lock();
    return vkQueuePresentKHR(m_handle, &presentInfo);
}

void VulkanQueue::wait() {
    std::shared_lock lock(m_mutex);
    vkQueueWaitIdle(m_handle);
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

