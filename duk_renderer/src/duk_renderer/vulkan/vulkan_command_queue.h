//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_RENDERER_VULKAN_QUEUE_H
#define DUK_RENDERER_VULKAN_QUEUE_H

#include <duk_macros/macros.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/command_queue.h>

namespace duk::renderer {

struct VulkanCommandQueueCreateInfo {
    VkDevice device;
    uint32_t familyIndex;
    uint32_t index;
};

class VulkanCommandQueue : public CommandQueue {
public:

    class Fence {
    public:
        Fence(VulkanCommandQueue& owner, VkFence fence);

        ~Fence();

         VkFence& handle();

    private:
        VulkanCommandQueue& m_owner;
        VkFence m_fence;
    };

private:
    struct Submission {
        std::vector<VkCommandBuffer> commandBuffers;
        std::shared_ptr<Fence> fence;
    };

public:

    explicit VulkanCommandQueue(const VulkanCommandQueueCreateInfo& commandQueueCreateInfo);

    ~VulkanCommandQueue() override;

    DUK_NO_DISCARD uint32_t index() const;

    DUK_NO_DISCARD uint32_t family_index() const;

    VkCommandBuffer allocate_command_buffer();

    std::shared_ptr<VulkanCommandQueue::Fence> submit(const VkSubmitInfo& submitInfo);

private:

    VkFence allocate_fence();

    void free_fence(VkFence fence);

    void trim_submissions();

private:
    VkDevice m_device;
    uint32_t m_familyIndex;
    uint32_t m_index;
    VkQueue m_queue;
    VkCommandPool m_commandPool;

    std::vector<Submission> m_submissions;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkFence> m_fences;
};

}

#endif //DUK_RENDERER_VULKAN_QUEUE_H
