/// 13/05/2023
/// vulkan_command.h

#ifndef DUK_RHI_VULKAN_COMMAND_H
#define DUK_RHI_VULKAN_COMMAND_H

#include <duk_rhi/command/command.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::rhi {

struct VulkanWaitDependency {
    uint32_t semaphoreCount;
    VkSemaphore* semaphores;
    VkPipelineStageFlags* stages;
};


struct VulkanCommandParams {
    VulkanWaitDependency waitDependency;
    VkSemaphore signalSemaphore;
    VkFence fence;
};

class VulkanSubmitter : public Submitter {
public:

    using SubmissionFunc = std::function<void(const VulkanCommandParams&)>;

    VulkanSubmitter(SubmissionFunc&& submissionFunc, bool signalsSemaphore, bool signalsFence, uint32_t frameCount, const uint32_t* currentFramePtr, VkDevice device);

    ~VulkanSubmitter() override;

    void submit(const VulkanWaitDependency& waitDependency);

    DUK_NO_DISCARD bool signals_semaphore() const;

    DUK_NO_DISCARD VkSemaphore semaphore();

    DUK_NO_DISCARD bool signals_fence() const;

    DUK_NO_DISCARD VkFence fence();

private:
    SubmissionFunc m_submissionFunc;
    const uint32_t* m_currentFramePtr;
    VkDevice m_device;
    std::vector<VkSemaphore> m_semaphores;
    std::vector<VkFence> m_fences;
};

}

#endif // DUK_RHI_VULKAN_COMMAND_H
