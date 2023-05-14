//
// Created by Ricardo on 13/04/2023.
//

#include <duk_renderer/vulkan/vulkan_command_queue.h>

namespace duk::renderer {

VulkanCommandQueue::VulkanCommandQueue(const VulkanCommandQueueCreateInfo& commandQueueCreateInfo) :
    m_device(commandQueueCreateInfo.device),
    m_familyIndex(commandQueueCreateInfo.familyIndex),
    m_index(commandQueueCreateInfo.index),
    m_currentFramePtr(commandQueueCreateInfo.currentFramePtr),
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

    VulkanCommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.commandQueue = this;
    commandBufferCreateInfo.currentFramePtr = m_currentFramePtr;
    commandBufferCreateInfo.frameCount = m_frameCount;

    m_tempCommandBuffer = std::make_unique<VulkanCommandBuffer>(commandBufferCreateInfo);
}

VulkanCommandQueue::~VulkanCommandQueue() {
    vkQueueWaitIdle(m_queue);
    m_commandBufferPool.reset();
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
}

uint32_t VulkanCommandQueue::index() const {
    return m_index;
}

uint32_t VulkanCommandQueue::family_index() const {
    return m_familyIndex;
}

VkCommandBuffer VulkanCommandQueue::allocate_command_buffer() {
    VkCommandBuffer commandBuffer = m_commandBufferPool->allocate();
    vkResetCommandBuffer(commandBuffer, 0);
    return commandBuffer;
}

void VulkanCommandQueue::free_command_buffer(VkCommandBuffer& commandBuffer) {
    m_commandBufferPool->free(commandBuffer);
}

void VulkanCommandQueue::submit(const VkSubmitInfo& submitInfo, VkFence* fence) {
    if (fence) {
        vkResetFences(m_device, 1, fence);
    }
    vkQueueSubmit(m_queue, 1, &submitInfo, *fence);
}

CommandBuffer* VulkanCommandQueue::next_command_buffer() {
    return m_tempCommandBuffer.get();
}

}