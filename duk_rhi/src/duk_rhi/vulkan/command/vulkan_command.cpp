/// 13/05/2023
/// vulkan_command.cpp

#include <duk_rhi/vulkan/command/vulkan_command.h>

namespace duk::rhi {

VulkanSubmitter::VulkanSubmitter(VulkanSubmitter::SubmissionFunc&& submissionFunc, bool signalsSemaphore, bool signalsFence, uint32_t frameCount, const uint32_t* currentFramePtr, VkDevice device) : m_submissionFunc(std::move(submissionFunc)),
                                                                                                                                                                                                      m_currentFramePtr(currentFramePtr),
                                                                                                                                                                                                      m_device(device) {
    if (signalsSemaphore) {
        m_semaphores.resize(frameCount);
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (auto& semaphore: m_semaphores) {
            vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &semaphore);
        }
    }

    if (signalsFence) {
        m_fences.resize(frameCount);
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (auto& fence: m_fences) {
            vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence);
        }
    }
}

VulkanSubmitter::~VulkanSubmitter() {
    for (auto& semaphore: m_semaphores) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }

    for (auto& fence: m_fences) {
        vkDestroyFence(m_device, fence, nullptr);
    }
}

void VulkanSubmitter::submit(const VulkanWaitDependency& waitDependency) {
    VulkanCommandParams params = {};
    params.waitDependency = waitDependency;
    if (signals_semaphore()) {
        params.signalSemaphore = semaphore();
    }
    if (signals_fence()) {
        params.fence = fence();
    }
    m_submissionFunc(params);
}

bool VulkanSubmitter::signals_semaphore() const {
    return !m_semaphores.empty();
}

VkSemaphore VulkanSubmitter::semaphore() {
    return m_semaphores[*m_currentFramePtr];
}

bool VulkanSubmitter::signals_fence() const {
    return !m_fences.empty();
}

VkFence VulkanSubmitter::fence() {
    return m_fences[*m_currentFramePtr];
}

}// namespace duk::rhi