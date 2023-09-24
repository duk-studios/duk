/// 28/05/2023
/// vulkan_buffer.h

#ifndef DUK_RHI_VULKAN_BUFFER_H
#define DUK_RHI_VULKAN_BUFFER_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/command/vulkan_command_queue.h>

#include <duk_hash/hash.h>

#include <vector>

namespace duk::rhi {

struct VulkanBufferMemoryCreateInfo {
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    VulkanCommandQueue* commandQueue;
    VkBufferUsageFlags usageFlags;
    size_t size;
};

class VulkanBufferMemory {
protected:

    explicit VulkanBufferMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags, VkBufferUsageFlags additionalUsageFlags);

public:

    virtual ~VulkanBufferMemory();

    virtual void write(void* src, size_t size, size_t offset) = 0;

    virtual void read(void* dst, size_t size, size_t offset) = 0;

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD size_t allocation_size() const;

    DUK_NO_DISCARD size_t alignment() const;

    DUK_NO_DISCARD VkBuffer handle() const;

    DUK_NO_DISCARD VkDeviceMemory memory() const;

protected:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    VulkanCommandQueue* m_commandQueue;
    VkBufferUsageFlags m_usageFlags;
    VkMemoryPropertyFlags m_memoryProperties;
    size_t m_size;

    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    size_t m_allocationSize;
    size_t m_alignment;
};

class VulkanBufferHostMemory : public VulkanBufferMemory {
public:
    explicit VulkanBufferHostMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo);

    ~VulkanBufferHostMemory() override;

    void write(void* src, size_t size, size_t offset) override;

    void read(void* dst, size_t size, size_t offset) override;

private:
    void map();

    void unmap();

private:
    void* m_mapped;
};

class VulkanBufferDeviceMemory : public VulkanBufferMemory {
public:
    explicit VulkanBufferDeviceMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo);

    ~VulkanBufferDeviceMemory() override;

    void write(void* src, size_t size, size_t offset) override;

    void read(void* dst, size_t size, size_t offset) override;
private:
    VkBufferUsageFlags m_requiredBufferUsageFlags;
};

struct VulkanBufferCreateInfo {
    Buffer::Type type;
    Buffer::UpdateFrequency updateFrequency;
    size_t elementCount;
    size_t elementSize;
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    VulkanCommandQueue* commandQueue;
    uint32_t imageCount;
};

class VulkanBuffer : public Buffer {
public:

    explicit VulkanBuffer(const VulkanBufferCreateInfo& bufferCreateInfo);

    ~VulkanBuffer() override;

    DUK_NO_DISCARD VkBuffer handle(uint32_t imageIndex);

    void update(uint32_t imageIndex);

    void create(uint32_t imageCount);

    void clean();

    void clean(uint32_t imageIndex);

    DUK_NO_DISCARD const uint8_t* read_ptr(size_t offset) const override;

    void read(void* dst, size_t size, size_t offset) override;

    DUK_NO_DISCARD uint8_t* write_ptr(size_t offset) override;

    void write(void* src, size_t size, size_t offset) override;

    void copy_from(Buffer* srcBuffer, size_t size, size_t srcOffset, size_t dstOffset) override;

    void resize(size_t elementCount) override;

    void flush() override;


    void invalidate() override;

    DUK_NO_DISCARD size_t element_count() const override;

    DUK_NO_DISCARD size_t element_size() const override;

    DUK_NO_DISCARD size_t byte_size() const override;

    DUK_NO_DISCARD Type type() const override;

    DUK_NO_DISCARD CommandQueue* command_queue() const override;

    DUK_NO_DISCARD duk::hash::Hash hash() const override;

private:

    void update_data_hash();

    std::unique_ptr<VulkanBufferMemory> create_buffer();

private:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    VulkanCommandQueue* m_commandQueue;
    Buffer::Type m_type;
    Buffer::UpdateFrequency m_updateFrequency;
    size_t m_elementCount;
    size_t m_elementSize;
    std::vector<std::unique_ptr<VulkanBufferMemory>> m_buffers;
    std::vector<duk::hash::Hash> m_bufferHashes;
    std::vector<uint8_t> m_data;
    duk::hash::Hash m_dataHash;

};


}

#endif // DUK_RHI_VULKAN_BUFFER_H

