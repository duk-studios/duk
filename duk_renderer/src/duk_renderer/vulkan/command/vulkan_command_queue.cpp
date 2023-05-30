//
// Created by Ricardo on 13/04/2023.
//

#include <duk_renderer/vulkan/command/vulkan_command_queue.h>

namespace duk::renderer {

std::vector<std::vector<VulkanCommandQueue*>> VulkanCommandQueue::s_commandQueuesFamilies;

VulkanCommandQueue* VulkanCommandQueue::queue_for_family_index(uint32_t queueFamilyIndex, uint32_t index) {
    assert(s_commandQueuesFamilies.size() > queueFamilyIndex);
    auto& familyQueues = s_commandQueuesFamilies.at(queueFamilyIndex);
    assert(familyQueues.size() > index);
    return familyQueues.at(index);
}

VulkanCommandQueue::VulkanCommandQueue(const VulkanCommandQueueCreateInfo& commandQueueCreateInfo) :
    m_device(commandQueueCreateInfo.device),
    m_familyIndex(commandQueueCreateInfo.familyIndex),
    m_index(commandQueueCreateInfo.index),
    m_currentFramePtr(commandQueueCreateInfo.currentFramePtr),
    m_currentImagePtr(commandQueueCreateInfo.currentImagePtr),
    m_frameCount(commandQueueCreateInfo.frameCount),
    m_queue(VK_NULL_HANDLE),
    m_commandPool(VK_NULL_HANDLE) {

    vkGetDeviceQueue(m_device, m_familyIndex, m_index, &m_queue);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = m_familyIndex;

    auto result = vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool);
    if (result != VK_SUCCESS){
        throw std::runtime_error("failed to create VkCommandPool");
    }

    VulkanCommandBufferPoolCreateInfo commandBufferPoolCreateInfo = {};
    commandBufferPoolCreateInfo.device = m_device;
    commandBufferPoolCreateInfo.commandPool = m_commandPool;

    m_commandBufferPool = std::make_unique<VulkanCommandBufferPool>(commandBufferPoolCreateInfo);

    // resets command buffers for this frame
    m_listener.listen(*commandQueueCreateInfo.prepareFrameEvent, [this](uint32_t){
        m_usedCommandBuffers = 0;
    });

    if (s_commandQueuesFamilies.size() <= m_familyIndex) {
        s_commandQueuesFamilies.resize(m_familyIndex + 1);
    }

    auto& familyQueues = s_commandQueuesFamilies[m_familyIndex];
    if (familyQueues.size() <= m_index) {
        familyQueues.resize(m_index + 1);
    }

    familyQueues[m_index] = this;
}

VulkanCommandQueue::~VulkanCommandQueue() {
    vkQueueWaitIdle(m_queue);
    m_commandBuffers.clear();
    m_commandBufferPool.reset();
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);

    s_commandQueuesFamilies[m_familyIndex][m_index] = nullptr;
}

uint32_t VulkanCommandQueue::index() const {
    return m_index;
}

uint32_t VulkanCommandQueue::family_index() const {
    return m_familyIndex;
}

VkQueue VulkanCommandQueue::handle() const {
    return m_queue;
}

VkCommandBuffer VulkanCommandQueue::allocate_command_buffer() {
    VkCommandBuffer commandBuffer = m_commandBufferPool->allocate();
    vkResetCommandBuffer(commandBuffer, 0);
    return commandBuffer;
}

void VulkanCommandQueue::free_command_buffer(VkCommandBuffer& commandBuffer) {
    m_commandBufferPool->free(commandBuffer);
}

void VulkanCommandQueue::submit(const VkSubmitInfo& submitInfo, VkFence fence) {
    if (fence != VK_NULL_HANDLE) {
        vkResetFences(m_device, 1, &fence);
    }

    vkQueueSubmit(m_queue, 1, &submitInfo, fence);

    if (fence == VK_NULL_HANDLE) {
        vkQueueWaitIdle(m_queue);
    }
}

CommandBuffer* VulkanCommandQueue::next_command_buffer() {
    auto nextCommandBufferIndex = m_usedCommandBuffers++;
    if (nextCommandBufferIndex < m_commandBuffers.size()){
        auto commandBuffer = m_commandBuffers[nextCommandBufferIndex].get();
        return commandBuffer;
    }

    VulkanCommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.device = m_device;
    commandBufferCreateInfo.commandQueue = this;
    commandBufferCreateInfo.currentFramePtr = m_currentFramePtr;
    commandBufferCreateInfo.currentImagePtr = m_currentImagePtr;
    commandBufferCreateInfo.frameCount = m_frameCount;
    return m_commandBuffers.emplace_back(std::make_unique<VulkanCommandBuffer>(commandBufferCreateInfo)).get();
}

}