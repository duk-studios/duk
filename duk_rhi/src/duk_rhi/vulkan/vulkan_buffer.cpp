/// 28/05/2023
/// vulkan_buffer.cpp

#include <duk_macros/assert.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>

#include <stdexcept>

namespace duk::rhi {

VulkanBuffer::VulkanBuffer(const VulkanBufferCreateInfo& createInfo)
    : m_device(createInfo.device)
    , m_size(createInfo.size)
    , m_usageFlags(createInfo.usageFlags)
    , m_memoryFlags(createInfo.memoryFlags)
    , m_indexType(createInfo.indexType) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = m_size;
    bufferInfo.usage = m_usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer: failed to create VkBuffer");
    }

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memReqs);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = createInfo.physicalDevice->find_memory_type(memReqs.memoryTypeBits, m_memoryFlags);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer: failed to allocate VkDeviceMemory");
    }

    if (vkBindBufferMemory(m_device, m_buffer, m_memory, 0) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer: failed to bind buffer memory");
    }
}

VulkanBuffer::~VulkanBuffer() {
    if (m_mapped) {
        vkUnmapMemory(m_device, m_memory);
    }
    if (m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
    }
    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_memory, nullptr);
    }
}

VkBuffer VulkanBuffer::handle() const {
    return m_buffer;
}

VkMemoryPropertyFlags VulkanBuffer::memory_flags() const {
    return m_memoryFlags;
}

VkIndexType VulkanBuffer::index_type() const {
    return m_indexType;
}

void VulkanBuffer::write(const void* src, size_t size, size_t offset) {
    DUK_ASSERT(m_mapped != nullptr);
    std::memcpy(static_cast<char*>(m_mapped) + offset, src, size);
}

void VulkanBuffer::read(void* dst, size_t size, size_t offset) const {
    DUK_ASSERT(m_memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    void* mapped = nullptr;
    if (vkMapMemory(m_device, m_memory, offset, size, 0, &mapped) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer::read: failed to map memory");
    }
    std::memcpy(dst, mapped, size);
    vkUnmapMemory(m_device, m_memory);
}

size_t VulkanBuffer::size() const {
    return m_size;
}

void* VulkanBuffer::map() {
    DUK_ASSERT(m_memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    if (!m_mapped) {
        if (vkMapMemory(m_device, m_memory, 0, VK_WHOLE_SIZE, 0, &m_mapped) != VK_SUCCESS) {
            throw std::runtime_error("VulkanBuffer::map: failed to map memory");
        }
    }
    return m_mapped;
}

void VulkanBuffer::unmap() {
    if (m_mapped) {
        vkUnmapMemory(m_device, m_memory);
        m_mapped = nullptr;
    }
}

void VulkanBuffer::flush(size_t offset, size_t size) {
    // Flushing is only necessary when memory is not HOST_COHERENT.
    if (m_memoryFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        return;
    }
    VkMappedMemoryRange range = {};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = m_memory;
    range.offset = static_cast<VkDeviceSize>(offset);
    range.size = static_cast<VkDeviceSize>(size);
    vkFlushMappedMemoryRanges(m_device, 1, &range);
}

void* VulkanBuffer::mapped_ptr() const {
    return m_mapped;
}

}// namespace duk::rhi