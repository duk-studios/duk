/// 13/05/2023
/// vulkan_command.cpp

#include <duk_renderer/vulkan/vulkan_command.h>

namespace duk::renderer {

VulkanSubmitter::VulkanSubmitter(VulkanSubmitter::SubmissionFunc&& submissionFunc, bool signalsSemaphore, bool signalsFence) :
    m_submissionFunc(std::move(submissionFunc)),
    m_signalsSemaphore(signalsSemaphore),
    m_signalsFence(signalsFence) {

}

void VulkanSubmitter::submit(const VulkanCommandParams& params) const {
    m_submissionFunc(params);
}

bool VulkanSubmitter::signals_semaphore() const {
    return m_signalsSemaphore;
}

bool VulkanSubmitter::signals_fence() const {
    return m_signalsFence;
}
}