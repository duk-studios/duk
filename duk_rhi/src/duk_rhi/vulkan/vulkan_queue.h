//
// Created by Ricardo on 03/05/2026.
//

#ifndef DUK_RHI_VULKAN_QUEUE_H
#define DUK_RHI_VULKAN_QUEUE_H

#include <duk_macros/macros.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <memory>
#include <mutex>

namespace duk::rhi {


struct VulkanQueueCreateInfo {
    VkDevice device;
    VkQueueFlags flags;
    uint32_t familyIndex;
    uint32_t queueIndex;
    /// Shared mutex for all VulkanQueue objects that wrap the same physical VkQueue.
    std::shared_ptr<std::mutex> mutex;
};

/// Thin wrapper around a single VkQueue. Thread-safe: concurrent submit calls
/// on wrappers sharing the same underlying VkQueue are serialised via a shared mutex.
class VulkanQueue {
public:
    explicit VulkanQueue(const VulkanQueueCreateInfo& createInfo);

    ~VulkanQueue() = default;

    /// Submits command buffers to the queue, guarded by the shared mutex.
    VkResult submit(uint32_t submitCount, const VkSubmitInfo* submitInfos, VkFence fence);

    DUK_NO_DISCARD VkQueue handle() const;
    DUK_NO_DISCARD uint32_t family_index() const;
    DUK_NO_DISCARD uint32_t queue_index() const;
    DUK_NO_DISCARD VkQueueFlags flags() const;

private:
    VkQueue m_handle;
    VkQueueFlags m_flags;
    uint32_t m_familyIndex;
    uint32_t m_queueIndex;
    std::shared_ptr<std::mutex> m_mutex;
};

}// namespace duk::rhi

#endif//DUK_RHI_VULKAN_QUEUE_H

