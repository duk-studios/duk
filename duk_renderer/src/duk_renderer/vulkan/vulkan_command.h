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

class VulkanSubmitter : public Submitter {
public:

    using SubmissionFunc = std::function<void(const VulkanCommandParams&)>;

    VulkanSubmitter(SubmissionFunc&& submissionFunc, bool signalsSemaphore, bool signalsFence);

    void submit(const VulkanCommandParams& params) const;

    DUK_NO_DISCARD bool signals_semaphore() const;

    DUK_NO_DISCARD bool signals_fence() const;

private:
    SubmissionFunc m_submissionFunc;
    bool m_signalsSemaphore;
    bool m_signalsFence;
};

}

#endif // DUK_RENDERER_VULKAN_COMMAND_H
