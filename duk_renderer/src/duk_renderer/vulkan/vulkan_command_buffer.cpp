/// 21/04/2023
/// vulkan_command_interface.cpp

#include <duk_renderer/vulkan/vulkan_command_buffer.h>
#include <duk_renderer/vulkan/vulkan_command_queue.h>
#include <duk_renderer/vulkan/vulkan_renderer.h>
#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_render_pass.h>
#include <duk_renderer/vulkan/vulkan_command.h>

namespace duk::renderer {

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanCommandBufferCreateInfo& commandInterfaceCreateInfo) :
    m_commandQueue(commandInterfaceCreateInfo.commandQueue),
    m_currentFramePtr(commandInterfaceCreateInfo.currentFramePtr) {
    m_commandBuffers.resize(commandInterfaceCreateInfo.frameCount);
    for (auto& commandBuffer : m_commandBuffers) {
        commandBuffer = m_commandQueue->allocate_command_buffer();
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    for (auto& commandBuffer : m_commandBuffers) {
        m_commandQueue->free_command_buffer(commandBuffer);
    }
}

void VulkanCommandBuffer::begin() {
    m_currentCommandBuffer = m_commandBuffers[*m_currentFramePtr];

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(m_currentCommandBuffer, &commandBufferBeginInfo);
}

void VulkanCommandBuffer::end() {
    vkEndCommandBuffer(m_currentCommandBuffer);
}

VulkanCommandQueue* VulkanCommandBuffer::command_queue() {
    return m_commandQueue;
}

VkCommandBuffer& VulkanCommandBuffer::handle() {
    return m_currentCommandBuffer;
}

void VulkanCommandBuffer::begin_render_pass(const CommandBuffer::RenderPassBeginInfo& renderPassBeginInfo) {

    auto vulkanFramebuffer = (VulkanFrameBuffer*)renderPassBeginInfo.frameBuffer;
    auto vulkanRenderPass = (VulkanRenderPass*)renderPassBeginInfo.renderPass;

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->handle();
    renderPassInfo.framebuffer = vulkanFramebuffer->handle(*m_currentFramePtr);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {vulkanFramebuffer->width(), vulkanFramebuffer->height()};

    VkClearValue clearValue = {};
    clearValue.color = {1.0f, 0.0f, 0.0f, 1.0f};
    clearValue.depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(m_currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::end_render_pass() {
    vkCmdEndRenderPass(m_currentCommandBuffer);
}

void VulkanCommandBuffer::render(const RenderMeshInfo& renderMeshInfo) {

}

void VulkanCommandBuffer::submit(const VulkanCommandParams& params) {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // wait for all dependencies of this command buffer
    if (params.waitDependency){
        submitInfo.waitSemaphoreCount = params.waitDependency->semaphoreCount;
        submitInfo.pWaitSemaphores = params.waitDependency->semaphores;
        submitInfo.pWaitDstStageMask = params.waitDependency->stages;
    }

    // only signal one semaphore per submission
    if (params.signalSemaphore){
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = params.signalSemaphore;
    }

    // only one command buffer per submission, for now...?
    submitInfo.pCommandBuffers = &m_currentCommandBuffer;
    submitInfo.commandBufferCount = 1;

    m_commandQueue->submit(submitInfo, params.fence);
}

}
