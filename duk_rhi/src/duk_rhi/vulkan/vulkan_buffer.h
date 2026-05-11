/// 28/05/2023
/// vulkan_buffer.h

#ifndef DUK_RHI_VULKAN_BUFFER_H
#define DUK_RHI_VULKAN_BUFFER_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>

#include <duk_macros/macros.h>

namespace duk::rhi {

struct VulkanBufferCreateInfo {
    size_t size;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryFlags;
    VkIndexType indexType{VK_INDEX_TYPE_MAX_ENUM}; ///< Only meaningful for index buffers.
    VkDevice device;
    const VulkanPhysicalDevice* physicalDevice;
};

class VulkanBuffer : public Buffer {
public:
    explicit VulkanBuffer(const VulkanBufferCreateInfo& createInfo);

    ~VulkanBuffer() override;

    // -----------------------------------------------------------------------
    // Internal Vulkan accessors
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD VkBuffer handle() const;

    DUK_NO_DISCARD VkMemoryPropertyFlags memory_flags() const;

    /// Only valid for index buffers (VK_BUFFER_USAGE_INDEX_BUFFER_BIT).
    DUK_NO_DISCARD VkIndexType index_type() const;

    /// Write directly into host-visible memory (no command buffer required).
    /// Caller must ensure memory_flags() includes VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT.
    void write(const void* src, size_t size, size_t offset);

    /// Read directly from host-visible memory.
    /// Caller must ensure memory_flags() includes VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT.
    void read(void* dst, size_t size, size_t offset) const;

    // -----------------------------------------------------------------------
    // Buffer public interface (read-only metadata)
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD size_t size() const override;

private:
    VkDevice m_device;
    size_t m_size;
    VkBufferUsageFlags m_usageFlags{0};
    VkMemoryPropertyFlags m_memoryFlags{0};
    VkIndexType m_indexType{VK_INDEX_TYPE_MAX_ENUM};
    VkBuffer m_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_BUFFER_H
