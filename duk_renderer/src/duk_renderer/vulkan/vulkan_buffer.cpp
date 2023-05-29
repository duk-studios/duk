/// 28/05/2023
/// vulkan_buffer.cpp

#include <duk_renderer/vulkan/vulkan_buffer.h>

#include <stdexcept>

namespace duk::renderer {

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

}

VulkanBufferMemory::VulkanBufferMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags) :
    m_device(vulkanBufferMemoryCreateInfo.device),
    m_physicalDevice(vulkanBufferMemoryCreateInfo.physicalDevice),
    m_queueFamilyIndex(vulkanBufferMemoryCreateInfo.queueFamilyIndex),
    m_usageFlags(vulkanBufferMemoryCreateInfo.usageFlags),
    m_memoryProperties(memoryPropertyFlags),
    m_size(vulkanBufferMemoryCreateInfo.size) {

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

VulkanBufferHostMemory::VulkanBufferHostMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo) :
    VulkanBufferMemory(vulkanBufferMemoryCreateInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
    map();
}

VulkanBufferHostMemory::~VulkanBufferHostMemory() {
    unmap();
}

void VulkanBufferHostMemory::write(void* src, size_t size, size_t offset) {
    assert(m_mapped);
    auto dst = (uint8_t*)m_mapped + offset;
    memcpy(dst, src, size);
}

void VulkanBufferHostMemory::read(void* dst, size_t size, size_t offset) {
    assert(m_mapped);
    auto src = (uint8_t*)m_mapped + offset;
    memcpy(dst, src, size);
}

void VulkanBufferHostMemory::map() {
    vkMapMemory(m_device, m_memory, 0, m_size, 0, &m_mapped);
}

void VulkanBufferHostMemory::unmap() {
    if (m_mapped) {
        vkUnmapMemory(m_device, m_memory);
        m_mapped = nullptr;
    }
}

VulkanBuffer::VulkanBuffer(const VulkanBufferCreateInfo& bufferCreateInfo) :
    m_device(bufferCreateInfo.device),
    m_physicalDevice(bufferCreateInfo.physicalDevice),
    m_queueFamilyIndex(bufferCreateInfo.queueFamilyIndex),
    m_type(bufferCreateInfo.type),
    m_updateFrequency(bufferCreateInfo.updateFrequency),
    m_size(bufferCreateInfo.size),
    m_elementSize(bufferCreateInfo.elementSize),
    m_data(m_size * m_elementSize, 0),
    m_dataHash(0) {

    create(bufferCreateInfo.imageCount);
}

VulkanBuffer::~VulkanBuffer() = default;

VkBuffer VulkanBuffer::handle(uint32_t imageIndex) {
    return m_buffers[imageIndex]->handle();
}

void VulkanBuffer::update(uint32_t imageIndex) {
    auto& bufferHash = m_bufferHashes[imageIndex];
    if (bufferHash != m_dataHash) {
        m_buffers[imageIndex]->write(m_data.data(), m_data.size(), 0);
        bufferHash = m_dataHash;
    }
}

void VulkanBuffer::create(uint32_t imageCount) {
    VulkanBufferMemoryCreateInfo bufferMemoryCreateInfo = {};
    bufferMemoryCreateInfo.device = m_device;
    bufferMemoryCreateInfo.physicalDevice = m_physicalDevice;
    bufferMemoryCreateInfo.queueFamilyIndex = m_queueFamilyIndex;
    bufferMemoryCreateInfo.size = m_data.size();
    bufferMemoryCreateInfo.usageFlags = detail::buffer_usage_from_type(m_type);

    m_buffers.resize(imageCount);
    for (auto& buffer : m_buffers) {
        switch (m_updateFrequency) {
            case UpdateFrequency::DYNAMIC:
                buffer = std::make_unique<VulkanBufferHostMemory>(bufferMemoryCreateInfo);
                break;
            default:
                throw std::invalid_argument("unhandled Buffer::UpdateFrequency");
        }
    }
    m_bufferHashes.resize(imageCount, 0);
}

void VulkanBuffer::clean() {
    m_buffers.clear();
    m_bufferHashes.clear();
}

void VulkanBuffer::clean(uint32_t imageIndex) {
    auto& buffer = m_buffers[imageIndex];
    if (buffer) {
        buffer.reset();
    }
}

void VulkanBuffer::read(void* dst, size_t size, size_t offset) {
    memcpy(dst, m_data.data() + offset, size);
}

void VulkanBuffer::write(void* src, size_t size, size_t offset) {
    memcpy(m_data.data() + offset, src, size);
    m_dataHash = 0;
    duk::hash::hash_combine(m_dataHash, m_data.data(), m_data.size());
}

size_t VulkanBuffer::size() const {
    return m_size;
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

}