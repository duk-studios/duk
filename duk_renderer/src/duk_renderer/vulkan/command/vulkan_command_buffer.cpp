/// 21/04/2023
/// vulkan_command_interface.cpp

#include <duk_renderer/vulkan/command/vulkan_command_buffer.h>
#include <duk_renderer/vulkan/command/vulkan_command_queue.h>
#include <duk_renderer/vulkan/command/vulkan_command.h>
#include <duk_renderer/vulkan/vulkan_renderer.h>
#include <duk_renderer/vulkan/vulkan_buffer.h>
#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_render_pass.h>
#include <duk_renderer/vulkan/pipeline/vulkan_pipeline.h>

namespace duk::renderer {

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanCommandBufferCreateInfo& commandBufferCreateInfo) :
    m_commandQueue(commandBufferCreateInfo.commandQueue),
    m_currentImagePtr(commandBufferCreateInfo.currentImagePtr),
    m_submitter([this](const auto& params) { submit(params); }, true, true, commandBufferCreateInfo.frameCount, commandBufferCreateInfo.currentFramePtr, commandBufferCreateInfo.device) {
    m_commandBuffers.resize(commandBufferCreateInfo.frameCount);
    for (auto& commandBuffer : m_commandBuffers) {
        commandBuffer = m_commandQueue->allocate_command_buffer();
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    for (auto& commandBuffer : m_commandBuffers) {
        m_commandQueue->free_command_buffer(commandBuffer);
    }
}

VulkanCommandQueue* VulkanCommandBuffer::command_queue() {
    return m_commandQueue;
}

VkCommandBuffer& VulkanCommandBuffer::handle() {
    return m_currentCommandBuffer;
}

void VulkanCommandBuffer::submit(const VulkanCommandParams& params) {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // wait for all dependencies of this command buffer
    if (params.waitDependency) {
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

void VulkanCommandBuffer::begin() {
    m_currentCommandBuffer = m_commandBuffers[*m_currentImagePtr];

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(m_currentCommandBuffer, &commandBufferBeginInfo);
}

void VulkanCommandBuffer::end() {
    vkEndCommandBuffer(m_currentCommandBuffer);
}

void VulkanCommandBuffer::begin_render_pass(const CommandBuffer::RenderPassBeginInfo& renderPassBeginInfo) {

    auto vulkanFramebuffer = (VulkanFrameBuffer*)renderPassBeginInfo.frameBuffer;
    auto vulkanRenderPass = (VulkanRenderPass*)renderPassBeginInfo.renderPass;

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->handle();
    renderPassInfo.framebuffer = vulkanFramebuffer->handle(*m_currentImagePtr);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {vulkanFramebuffer->width(), vulkanFramebuffer->height()};

    VkClearValue clearColor = {};
    clearColor.color = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::end_render_pass() {
    vkCmdEndRenderPass(m_currentCommandBuffer);
}

void VulkanCommandBuffer::bind_pipeline(Pipeline* pipeline) {
    auto vulkanPipeline = dynamic_cast<VulkanPipeline*>(pipeline);
    vkCmdBindPipeline(m_currentCommandBuffer, vulkanPipeline->bind_point(), vulkanPipeline->handle());
}

void VulkanCommandBuffer::bind_vertex_buffer(Buffer* buffer) {
    assert(buffer->type() == Buffer::Type::VERTEX);

    auto vulkanBuffer = dynamic_cast<VulkanBuffer*>(buffer);
    vulkanBuffer->update(*m_currentImagePtr);

    VkDeviceSize offsets[] = {0};
    VkBuffer buffers[] = {vulkanBuffer->handle(*m_currentImagePtr)};

    vkCmdBindVertexBuffers(m_currentCommandBuffer, 0, 1, buffers, offsets);
}

void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance) {
    vkCmdDraw(m_currentCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

Submitter* VulkanCommandBuffer::submitter_ptr() {
    return &m_submitter;
}

}
