/// vulkan_deletion_queue.cpp

#include <duk_rhi/vulkan/vulkan_deletion_queue.h>

namespace duk::rhi {

VulkanDeletionQueue::VulkanDeletionQueue(uint32_t framesInFlight)
    : m_framesInFlight(framesInFlight) {
}

VulkanDeletionQueue::~VulkanDeletionQueue() {
}

void VulkanDeletionQueue::flush(uint32_t frameCounter) {
    std::vector<DeletionEntry> pendingEntries;
    pendingEntries.reserve(m_entries.size());
    for (auto entry : m_entries) {
        if (frameCounter >= entry.deletionFrame) {
            entry.deleter();
        }
        else {
            pendingEntries.push_back(entry);
        }
    }
    std::swap(m_entries, pendingEntries);
}

bool VulkanDeletionQueue::empty() const {
    return m_entries.empty();
}

std::size_t VulkanDeletionQueue::size() const {
    return m_entries.size();
}

}// namespace duk::rhi

