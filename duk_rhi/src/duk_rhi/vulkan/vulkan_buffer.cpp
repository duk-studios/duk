/// 28/05/2023
/// vulkan_buffer.cpp

#include <duk_macros/assert.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>

#include <stdexcept>

namespace duk::rhi {

VulkanBuffer::VulkanBuffer(const VulkanBufferCreateInfo& ci)
    : m_device(ci.device)
    , m_physicalDevice(ci.physicalDevice)
    , m_size(ci.size)
    , m_usageFlags(ci.usageFlags)
    , m_memoryFlags(ci.memoryFlags)
    , m_indexType(ci.indexType) {

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
    allocInfo.memoryTypeIndex = m_physicalDevice->find_memory_type(memReqs.memoryTypeBits, m_memoryFlags);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer: failed to allocate VkDeviceMemory");
    }

    if (vkBindBufferMemory(m_device, m_buffer, m_memory, 0) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer: failed to bind buffer memory");
    }
}

VulkanBuffer::~VulkanBuffer() {
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
    DUK_ASSERT(m_memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    void* mapped = nullptr;
    if (vkMapMemory(m_device, m_memory, offset, size, 0, &mapped) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer::write: failed to map memory");
    }
    memcpy(mapped, src, size);
    vkUnmapMemory(m_device, m_memory);
}

void VulkanBuffer::read(void* dst, size_t size, size_t offset) const {
    DUK_ASSERT(m_memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    void* mapped = nullptr;
    if (vkMapMemory(m_device, m_memory, offset, size, 0, &mapped) != VK_SUCCESS) {
        throw std::runtime_error("VulkanBuffer::read: failed to map memory");
    }
    memcpy(dst, mapped, size);
    vkUnmapMemory(m_device, m_memory);
}

size_t VulkanBuffer::size() const {
    return m_size;
}

}// namespace duk::rhi