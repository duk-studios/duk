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

    /// Maps the entire buffer into CPU address space persistently and returns the base pointer.
    /// Caller must ensure memory_flags() includes VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT.
    void* map(size_t offset, size_t size);

    /// Unmaps a previously mapped buffer.  No-op if not currently mapped.
    void unmap();

    /// Flushes the given byte range of mapped memory to make CPU writes visible to the GPU.
    /// No-op when the memory is HOST_COHERENT
    void flush(size_t offset, size_t size) const;

    /// Invalidates the given byte range of mapped memory to make GPU writes visible to the CPU.
    /// No-op when the memory is HOST_COHERENT
    void invalidate(size_t offset, size_t size) const;

    // -----------------------------------------------------------------------
    // Buffer public interface (read-only metadata)
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD size_t size() const override;


    DUK_NO_DISCARD void* data() const override;

private:
    VkDevice m_device;
    size_t m_size;
    VkBufferUsageFlags m_usageFlags{0};
    VkMemoryPropertyFlags m_memoryFlags{0};
    VkBuffer m_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
    void* m_mapped{nullptr};
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_BUFFER_H
