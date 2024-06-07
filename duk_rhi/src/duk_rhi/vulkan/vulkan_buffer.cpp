/// 28/05/2023
/// vulkan_buffer.cpp

#include <duk_macros/assert.h>
#include <duk_rhi/vulkan/command/vulkan_command_queue.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_resource_manager.h>

#include <stdexcept>

namespace duk::rhi {

namespace detail {

static VkBufferUsageFlags buffer_usage_from_type(Buffer::Type type) {
    VkBufferUsageFlags usageFlags = 0;
    switch (type) {
        case Buffer::Type::INDEX_16:
        case Buffer::Type::INDEX_32:
            usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        case Buffer::Type::VERTEX:
            usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case Buffer::Type::UNIFORM:
            usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        case Buffer::Type::STORAGE:
            usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            break;
        default:
            throw std::invalid_argument("unhandled Buffer::Type");
    }
    return usageFlags;
}

}// namespace detail

VulkanBufferMemory::VulkanBufferMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags, VkBufferUsageFlags additionalUsageFlags)
    : m_device(vulkanBufferMemoryCreateInfo.device)
    , m_physicalDevice(vulkanBufferMemoryCreateInfo.physicalDevice)
    , m_commandQueue(vulkanBufferMemoryCreateInfo.commandQueue)
    , m_usageFlags(vulkanBufferMemoryCreateInfo.usageFlags | additionalUsageFlags)
    , m_memoryProperties(memoryPropertyFlags)
    , m_size(vulkanBufferMemoryCreateInfo.size) {
    VkBufferCreateInfo vkBufferCreateInfo = {};
    vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkBufferCreateInfo.size = m_size;
    vkBufferCreateInfo.usage = m_usageFlags;
    vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto result = vkCreateBuffer(m_device, &vkBufferCreateInfo, nullptr, &m_buffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkBuffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    m_alignment = memRequirements.alignment;
    m_allocationSize = memRequirements.size;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = m_allocationSize;
    allocInfo.memoryTypeIndex = m_physicalDevice->find_memory_type(memRequirements.memoryTypeBits, m_memoryProperties);

    result = vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate VkDeviceMemory");
    }

    result = vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to bind VkBuffer with VkDeviceMemory");
    }
}

VulkanBufferMemory::~VulkanBufferMemory() {
    if (m_buffer) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
    }

    if (m_memory) {
        vkFreeMemory(m_device, m_memory, nullptr);
    }
}

size_t VulkanBufferMemory::size() const {
    return m_size;
}

size_t VulkanBufferMemory::allocation_size() const {
    return m_allocationSize;
}

size_t VulkanBufferMemory::alignment() const {
    return m_alignment;
}

VkBuffer VulkanBufferMemory::handle() const {
    return m_buffer;
}

VkDeviceMemory VulkanBufferMemory::memory() const {
    return m_memory;
}

VulkanBufferHostMemory::VulkanBufferHostMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo)
    : VulkanBufferMemory(vulkanBufferMemoryCreateInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 0)
    , m_mapped(nullptr) {
}

VulkanBufferHostMemory::~VulkanBufferHostMemory() = default;

void VulkanBufferHostMemory::write(void* src, size_t size, size_t offset) {
    map();
    DUK_ASSERT(m_mapped);
    auto dst = (uint8_t*)m_mapped + offset;
    memcpy(dst, src, size);
    unmap();
}

void VulkanBufferHostMemory::read(void* dst, size_t size, size_t offset) {
    map();
    auto src = (uint8_t*)m_mapped + offset;
    memcpy(dst, src, size);
    unmap();
}

void VulkanBufferHostMemory::map() {
    if (!m_mapped) {
        auto result = vkMapMemory(m_device, m_memory, 0, m_size, 0, &m_mapped);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to map VkDeviceMemory");
        }
    }
}

void VulkanBufferHostMemory::unmap() {
    if (m_mapped) {
        vkUnmapMemory(m_device, m_memory);
        m_mapped = nullptr;
    }
}

VulkanBufferDeviceMemory::VulkanBufferDeviceMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo)
    : VulkanBufferMemory(vulkanBufferMemoryCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
    , m_requiredBufferUsageFlags(vulkanBufferMemoryCreateInfo.usageFlags) {
}

VulkanBufferDeviceMemory::~VulkanBufferDeviceMemory() = default;

void VulkanBufferDeviceMemory::write(void* src, size_t size, size_t offset) {
    VulkanBufferMemoryCreateInfo bufferHostMemoryCreateInfo = {};
    bufferHostMemoryCreateInfo.device = m_device;
    bufferHostMemoryCreateInfo.physicalDevice = m_physicalDevice;
    bufferHostMemoryCreateInfo.usageFlags = m_requiredBufferUsageFlags | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferHostMemoryCreateInfo.commandQueue = m_commandQueue;
    bufferHostMemoryCreateInfo.size = size;

    VulkanBufferHostMemory bufferHostMemory(bufferHostMemoryCreateInfo);

    bufferHostMemory.write(src, size, 0);

    m_commandQueue->submit([&](VkCommandBuffer commandBuffer) {
        VkBufferCopy bufferCopyRegion = {};
        bufferCopyRegion.size = size;
        bufferCopyRegion.srcOffset = 0;
        bufferCopyRegion.dstOffset = offset;

        vkCmdCopyBuffer(commandBuffer, bufferHostMemory.handle(), m_buffer, 1, &bufferCopyRegion);
    });
}

void VulkanBufferDeviceMemory::read(void* dst, size_t size, size_t offset) {
    VulkanBufferMemoryCreateInfo bufferHostMemoryCreateInfo = {};
    bufferHostMemoryCreateInfo.device = m_device;
    bufferHostMemoryCreateInfo.physicalDevice = m_physicalDevice;
    bufferHostMemoryCreateInfo.usageFlags = m_requiredBufferUsageFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferHostMemoryCreateInfo.commandQueue = m_commandQueue;
    bufferHostMemoryCreateInfo.size = size;

    VulkanBufferHostMemory bufferHostMemory(bufferHostMemoryCreateInfo);

    m_commandQueue->submit([&](VkCommandBuffer commandBuffer) {
        VkBufferCopy bufferCopyRegion = {};
        bufferCopyRegion.size = size;
        bufferCopyRegion.srcOffset = offset;
        bufferCopyRegion.dstOffset = 0;

        vkCmdCopyBuffer(commandBuffer, m_buffer, bufferHostMemory.handle(), 1, &bufferCopyRegion);
    });

    bufferHostMemory.read(dst, size, offset);
}

VulkanBuffer::VulkanBuffer(const VulkanBufferCreateInfo& bufferCreateInfo)
    : m_device(bufferCreateInfo.device)
    , m_physicalDevice(bufferCreateInfo.physicalDevice)
    , m_commandQueue(bufferCreateInfo.commandQueue)
    , m_type(bufferCreateInfo.type)
    , m_updateFrequency(bufferCreateInfo.updateFrequency)
    , m_elementCount(bufferCreateInfo.elementCount)
    , m_elementSize(bufferCreateInfo.elementSize)
    , m_data(m_elementCount * m_elementSize, 0)
    , m_resourceManager(bufferCreateInfo.resourceManager) {
    create(bufferCreateInfo.imageCount);
}

VulkanBuffer::~VulkanBuffer() = default;

VkBuffer VulkanBuffer::handle(uint32_t imageIndex) {
    return m_buffers[imageIndex]->handle();
}

VkBuffer VulkanBuffer::handle(uint32_t imageIndex) const {
    return m_buffers[imageIndex]->handle();
}

void VulkanBuffer::update(uint32_t imageIndex) {
    auto& buffer = m_buffers[imageIndex];
    if (buffer->size() < m_data.size()) {
        // we need to resize this buffer
        buffer = create_buffer();
    }
    buffer->write(m_data.data(), m_data.size(), 0);
}

void VulkanBuffer::create(uint32_t imageCount) {
    m_buffers.resize(imageCount);
    for (auto& buffer: m_buffers) {
        buffer = create_buffer();
    }
    m_resourceManager->schedule_for_update(this);
}

void VulkanBuffer::clean() {
    m_buffers.clear();
}

void VulkanBuffer::clean(uint32_t imageIndex) {
    auto& buffer = m_buffers[imageIndex];
    if (buffer) {
        buffer.reset();
    }
}

const uint8_t* VulkanBuffer::read_ptr(size_t offset) const {
    DUK_ASSERT(offset < m_data.size());
    return m_data.data() + offset;
}

void VulkanBuffer::read(void* dst, size_t size, size_t offset) {
    DUK_ASSERT(offset + size <= m_data.size());
    memcpy(dst, m_data.data() + offset, size);
}

uint8_t* VulkanBuffer::write_ptr(size_t offset) {
    DUK_ASSERT(offset < m_data.size());
    return m_data.data() + offset;
}

void VulkanBuffer::write(const void* src, size_t size, size_t offset) {
    DUK_ASSERT(offset + size <= m_data.size());
    memcpy(m_data.data() + offset, src, size);
}

void VulkanBuffer::copy_from(Buffer* srcBuffer, size_t size, size_t srcOffset, size_t dstOffset) {
    auto buffer = dynamic_cast<VulkanBuffer*>(srcBuffer);
    DUK_ASSERT(buffer);
    DUK_ASSERT(size + dstOffset <= m_data.size());
    DUK_ASSERT(size + srcOffset <= buffer->m_data.size());

    auto srcPtr = buffer->m_data.data() + srcOffset;
    auto dstPtr = m_data.data() + dstOffset;
    memcpy(dstPtr, srcPtr, size);
}

void VulkanBuffer::flush() {
    m_resourceManager->schedule_for_update(this);
}

size_t VulkanBuffer::element_count() const {
    return m_elementCount;
}

size_t VulkanBuffer::element_size() const {
    return m_elementSize;
}

size_t VulkanBuffer::byte_size() const {
    return m_data.size();
}

Buffer::Type VulkanBuffer::type() const {
    return m_type;
}

Buffer::UpdateFrequency VulkanBuffer::update_frequency() const {
    return m_updateFrequency;
}

CommandQueue* VulkanBuffer::command_queue() const {
    return m_commandQueue;
}

std::unique_ptr<VulkanBufferMemory> VulkanBuffer::create_buffer() {
    VulkanBufferMemoryCreateInfo bufferMemoryCreateInfo = {};
    bufferMemoryCreateInfo.device = m_device;
    bufferMemoryCreateInfo.physicalDevice = m_physicalDevice;
    bufferMemoryCreateInfo.commandQueue = m_commandQueue;
    bufferMemoryCreateInfo.size = m_elementCount * m_elementSize;
    bufferMemoryCreateInfo.usageFlags = detail::buffer_usage_from_type(m_type);

    switch (m_updateFrequency) {
        case UpdateFrequency::DYNAMIC:
            return std::make_unique<VulkanBufferHostMemory>(bufferMemoryCreateInfo);
        case UpdateFrequency::STATIC:
            return std::make_unique<VulkanBufferDeviceMemory>(bufferMemoryCreateInfo);
        default:
            throw std::invalid_argument("unhandled Buffer::UpdateFrequency");
    }
}

}// namespace duk::rhi