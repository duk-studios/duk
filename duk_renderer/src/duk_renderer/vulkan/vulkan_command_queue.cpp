//
// Created by Ricardo on 13/04/2023.
//

#include <duk_renderer/vulkan/vulkan_command_queue.h>

#include <vector>

namespace duk::renderer {

VulkanCommandQueue::Fence::Fence(VulkanCommandQueue& owner, VkFence fence) :
    m_owner(owner),
    m_fence(fence) {

}

VulkanCommandQueue::Fence::~Fence() {
    m_owner.free_fence(m_fence);
}

VkFence& VulkanCommandQueue::Fence::handle() {
    return m_fence;
}

VulkanCommandQueue::VulkanCommandQueue(const VulkanCommandQueueCreateInfo& commandQueueCreateInfo) :
    m_device(commandQueueCreateInfo.device),
    m_familyIndex(commandQueueCreateInfo.familyIndex),
    m_index(commandQueueCreateInfo.index),
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
}

VulkanCommandQueue::~VulkanCommandQueue() {
    vkQueueWaitIdle(m_queue);
    trim_submissions();
    if (!m_commandBuffers.empty()){
        vkFreeCommandBuffers(m_device, m_commandPool, m_commandBuffers.size(), m_commandBuffers.data());
    }
    for (auto fence : m_fences){
        vkDestroyFence(m_device, fence, nullptr);
    }
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
}

uint32_t VulkanCommandQueue::index() const {
    return m_index;
}

uint32_t VulkanCommandQueue::family_index() const {
    return m_familyIndex;
}

VkCommandBuffer VulkanCommandQueue::allocate_command_buffer() {
    trim_submissions();

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    if (!m_commandBuffers.empty()) {
        commandBuffer = m_commandBuffers.back();
        m_commandBuffers.pop_back();

        vkResetCommandBuffer(commandBuffer, 0);

        return commandBuffer;
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, &commandBuffer);

    return commandBuffer;
}

std::shared_ptr<VulkanCommandQueue::Fence> VulkanCommandQueue::submit(const VkSubmitInfo& submitInfo) {
    auto fence = allocate_fence();

    Submission submission = {};
    submission.commandBuffers.insert(submission.commandBuffers.end(), submitInfo.pCommandBuffers, submitInfo.pCommandBuffers + submitInfo.commandBufferCount);
    submission.fence = std::make_shared<Fence>(*this, fence);

    m_submissions.push_back(submission);

    vkQueueSubmit(m_queue, 1, &submitInfo, fence);

    return submission.fence;
}

VkFence VulkanCommandQueue::allocate_fence() {

    VkFence fence = VK_NULL_HANDLE;
    if (!m_fences.empty()){
        fence = m_fences.back();
        m_fences.pop_back();
        return fence;
    }

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence);

    return fence;
}

void VulkanCommandQueue::free_fence(VkFence fence) {
    m_fences.push_back(fence);
}

void VulkanCommandQueue::trim_submissions() {
    std::vector<Submission> activeSubmissions;
    for (auto& submission : m_submissions){
        auto result = vkWaitForFences(m_device, 1, &submission.fence->handle(), VK_TRUE, 0);
        if (result == VK_TIMEOUT){
            activeSubmissions.emplace_back(submission);
            continue;
        }
        m_commandBuffers.insert(m_commandBuffers.end(), submission.commandBuffers.begin(), submission.commandBuffers.end());
    }
    m_submissions.swap(activeSubmissions);
}

}