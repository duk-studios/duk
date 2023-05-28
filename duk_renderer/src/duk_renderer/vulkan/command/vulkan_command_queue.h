//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_RENDERER_VULKAN_QUEUE_H
#define DUK_RENDERER_VULKAN_QUEUE_H

#include <duk_macros/macros.h>
#include <duk_renderer/command/command_queue.h>
#include <duk_renderer/vulkan/command/vulkan_command_buffer.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_resource_pool.h>
#include <duk_renderer/vulkan/vulkan_events.h>

namespace duk::renderer {

class VulkanRenderer;

struct VulkanCommandQueueCreateInfo {
    VkDevice device;
    uint32_t familyIndex;
    uint32_t index;
    const uint32_t* currentFramePtr;
    const uint32_t* currentImagePtr;
    uint32_t frameCount;
    uint32_t imageCount;
    VulkanPrepareFrameEvent* prepareFrameEvent;
};

class VulkanCommandQueue : public CommandQueue {
public:

    explicit VulkanCommandQueue(const VulkanCommandQueueCreateInfo& commandQueueCreateInfo);

    ~VulkanCommandQueue() override;

    DUK_NO_DISCARD uint32_t index() const;

    DUK_NO_DISCARD uint32_t family_index() const;

    VkCommandBuffer allocate_command_buffer();

    void free_command_buffer(VkCommandBuffer& commandBuffer);

    void submit(const VkSubmitInfo& submitInfo, VkFence* fence);

    // CommandQueue overrides
    DUK_NO_DISCARD CommandBuffer* next_command_buffer() override;

private:
    VkDevice m_device;
    uint32_t m_familyIndex;
    uint32_t m_index;
    const uint32_t* m_currentFramePtr;
    const uint32_t* m_currentImagePtr;
    uint32_t m_frameCount;
    VkQueue m_queue;
    VkCommandPool m_commandPool;
    std::unique_ptr<VulkanCommandBufferPool> m_commandBufferPool;
    std::vector<std::unique_ptr<VulkanCommandBuffer>> m_commandBuffers;
    uint32_t m_usedCommandBuffers;
    duk::events::EventListener m_listener;
};

}

#endif //DUK_RENDERER_VULKAN_QUEUE_H
