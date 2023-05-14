/// 13/05/2023
/// vulkan_command.h

#ifndef DUK_RENDERER_VULKAN_COMMAND_H
#define DUK_RENDERER_VULKAN_COMMAND_H

#include <duk_renderer/command.h>
#include <duk_renderer/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::renderer {

struct VulkanWaitDependency {
    uint32_t semaphoreCount;
    VkSemaphore* semaphores;
    VkPipelineStageFlags* stages;
};


struct VulkanCommandParams {
    VulkanWaitDependency* waitDependency;
    VkSemaphore* signalSemaphore;
    VkFence* fence;
};

class VulkanCommand {
public:
    virtual ~VulkanCommand() = default;

    virtual void submit(const VulkanCommandParams& params) = 0;
};


}

#endif // DUK_RENDERER_VULKAN_COMMAND_H

