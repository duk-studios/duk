/// 14/05/2023
/// vulkan_resource_pool.cpp

#include <duk_renderer/vulkan/vulkan_resource_pool.h>

#include <stdexcept>

namespace duk::renderer {

/// VkFence
VulkanFencePool::VulkanFencePool(const VulkanFencePoolCreateInfo& fencePoolCreateInfo) :
    m_device(fencePoolCreateInfo.device),
    m_flags(fencePoolCreateInfo.flags) {

}

VulkanFencePool::~VulkanFencePool() {
    clean();
}

void VulkanFencePool::expand(size_t n) {
    std::vector<VkFence> fences(n);

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = m_flags;
    for (auto& fence : fences) {
        auto result = vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create VkFence");
        }
    }
    m_resources.insert(m_resources.end(), fences.begin(), fences.end());
}

void VulkanFencePool::clean() {
    for (auto& fence : m_resources) {
        vkDestroyFence(m_device, fence, nullptr);
    }
    m_resources.clear();
}

/// VkSemaphore
VulkanSemaphorePool::VulkanSemaphorePool(const VulkanSemaphorePoolCreateInfo& semaphorePoolCreateInfo) :
    m_device(semaphorePoolCreateInfo.device) {

}

VulkanSemaphorePool::~VulkanSemaphorePool() {
    clean();
}

void VulkanSemaphorePool::expand(size_t n) {
    std::vector<VkSemaphore> semaphores(n);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (auto& semaphore : semaphores) {
        auto result = vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &semaphore);
        if (result != VK_SUCCESS){
            throw std::runtime_error("failed to create VkSemaphore");
        }
    }
    m_resources.insert(m_resources.end(), semaphores.begin(), semaphores.end());
}

void VulkanSemaphorePool::clean() {
    for (auto& semaphore : m_resources) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    m_resources.clear();
}

/// VkCommandBuffer
VulkanCommandBufferPool::VulkanCommandBufferPool(const VulkanCommandBufferPoolCreateInfo& commandBufferPoolCreateInfo) :
    m_device(commandBufferPoolCreateInfo.device),
    m_commandPool(commandBufferPoolCreateInfo.commandPool) {

}

VulkanCommandBufferPool::~VulkanCommandBufferPool() {
    clean();
}

void VulkanCommandBufferPool::expand(size_t n) {
    std::vector<VkCommandBuffer> commandBuffers(n);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandBufferCount = n;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    auto result = vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, commandBuffers.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate VkCommandBuffer");
    }

    m_resources.insert(m_resources.end(), commandBuffers.begin(), commandBuffers.end());
}

void VulkanCommandBufferPool::clean() {
    if (!m_resources.empty()) {
        vkFreeCommandBuffers(m_device, m_commandPool, m_resources.size(), m_resources.data());
    }
    m_resources.clear();
}

}