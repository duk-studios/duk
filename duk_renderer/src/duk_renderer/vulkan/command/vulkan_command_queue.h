//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_RENDERER_VULKAN_QUEUE_H
#define DUK_RENDERER_VULKAN_QUEUE_H

#include <duk_macros/macros.h>
#include <duk_renderer/command/command_queue.h>
#include <duk_renderer/vulkan/command/vulkan_command_buffer.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_handle_pool.h>
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

    static VulkanCommandQueue* queue_for_family_index(uint32_t queueFamilyIndex, uint32_t index);

    DUK_NO_DISCARD uint32_t index() const;

    DUK_NO_DISCARD uint32_t family_index() const;

    DUK_NO_DISCARD VkQueue handle() const;

    DUK_NO_DISCARD VkCommandBuffer allocate_command_buffer();

    void free_command_buffer(VkCommandBuffer& commandBuffer);

    void submit(const VkSubmitInfo& submitInfo, VkFence fence);

    /// Takes in a function that has a VkCommandBuffer as a parameter.
    /// Will call vkBeginCommandBuffer before and vkEndCommandBuffer after invoking the given function
    /// the given function is executed before returning, also guarantees that function is executed
    /// on the underlying thread to avoid data races in the VkCommandPool that is being used
    template<typename F, std::enable_if_t<std::is_void_v<std::invoke_result_t<F, VkCommandBuffer>>, int> = 0>
    void submit(F&& func, VkFence fence) {

        auto submission = [this, taskFunc = std::forward<F>(func), fence] {
            auto commandBuffer = allocate_command_buffer();

            VkCommandBufferBeginInfo commandBufferBeginInfo = {};
            commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

            taskFunc(commandBuffer);

            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            submit(submitInfo, fence);

            free_command_buffer(commandBuffer);
        };

        if (m_taskQueue.owns_current_thread()) {
            submission();
        }
        else {
            m_taskQueue.template enqueue(submission).wait();
        }
    }

    // CommandQueue overrides
    DUK_NO_DISCARD CommandBuffer* next_command_buffer() override;

private:
    static std::vector<std::vector<VulkanCommandQueue*>> s_commandQueuesFamilies;
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
