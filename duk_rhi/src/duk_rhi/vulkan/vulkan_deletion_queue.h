/// vulkan_deletion_queue.h

#ifndef DUK_RHI_VULKAN_DELETION_QUEUE_H
#define DUK_RHI_VULKAN_DELETION_QUEUE_H

#include <duk_macros/macros.h>

#include <functional>
#include <vector>

namespace duk::rhi {

/// A deferred-deletion container for heap-allocated GPU resources.
///
/// Call push(ptr) to hand ownership of a raw pointer to the queue.
/// Call flush() once a per-frame GPU fence has been signalled to destroy
/// every pending entry in push-order via the correct typed delete.
///
/// VulkanDeletionQueue is intentionally NOT copyable and NOT thread-safe.
class VulkanDeletionQueue {
public:
    explicit VulkanDeletionQueue(uint32_t framesInFlight);

    ~VulkanDeletionQueue();

    VulkanDeletionQueue(const VulkanDeletionQueue&) = delete;
    VulkanDeletionQueue& operator=(const VulkanDeletionQueue&) = delete;

    VulkanDeletionQueue(VulkanDeletionQueue&&) = default;
    VulkanDeletionQueue& operator=(VulkanDeletionQueue&&) = default;

    /// Enqueue a heap-allocated resource for deferred destruction.
    /// The queue takes ownership of ptr; flush() will call `delete ptr`.
    template<typename T>
    void push(T* ptr, uint32_t currentFrame) {
        m_entries.emplace_back(currentFrame + m_framesInFlight, [ptr] {
            delete ptr;
        });
    }

    /// Destroy all pending resources by calling their typed deleters.
    /// Safe to call at any time; typically called after a per-frame fence wait.
    void flush(uint32_t frameCounter);

    DUK_NO_DISCARD bool empty() const;

    DUK_NO_DISCARD std::size_t size() const;

private:
    struct DeletionEntry {
        uint32_t deletionFrame{0};
        std::function<void()> deleter;
    };

    uint32_t m_framesInFlight;
    std::vector<DeletionEntry> m_entries;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_DELETION_QUEUE_H

