/// 21/04/2023
/// vulkan_command_interface.cpp

#include <duk_rhi/vulkan/command/vulkan_command.h>
#include <duk_rhi/vulkan/command/vulkan_command_buffer.h>
#include <duk_rhi/vulkan/command/vulkan_command_queue.h>
#include <duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_pipeline_flags.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_tools/fixed_vector.h>

namespace duk::rhi {

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanCommandBufferCreateInfo& commandBufferCreateInfo)
    : m_commandQueue(commandBufferCreateInfo.commandQueue)
    , m_currentImagePtr(commandBufferCreateInfo.currentImagePtr)
    , m_submitter(
              [this](const auto& params) {
                  submit(params);
              },
              true, true, commandBufferCreateInfo.frameCount, commandBufferCreateInfo.currentFramePtr, commandBufferCreateInfo.device)
    , m_currentPipelineLayout(VK_NULL_HANDLE) {
    m_commandBuffers.resize(commandBufferCreateInfo.frameCount);
    for (auto& commandBuffer: m_commandBuffers) {
        commandBuffer = m_commandQueue->allocate_command_buffer();
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    for (auto& commandBuffer: m_commandBuffers) {
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
    if (params.waitDependency.semaphoreCount > 0) {
        submitInfo.waitSemaphoreCount = params.waitDependency.semaphoreCount;
        submitInfo.pWaitSemaphores = params.waitDependency.semaphores;
        submitInfo.pWaitDstStageMask = params.waitDependency.stages;
    }

    // only signal one semaphore per submission
    if (params.signalSemaphore) {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &params.signalSemaphore;
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

void VulkanCommandBuffer::begin_render_pass(RenderPass* renderPass, FrameBuffer* frameBuffer) {
    auto vulkanFramebuffer = (VulkanFrameBuffer*)frameBuffer;
    auto vulkanRenderPass = (VulkanRenderPass*)renderPass;

    auto imageIndex = *m_currentImagePtr;

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->handle();
    renderPassInfo.framebuffer = vulkanFramebuffer->handle(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {vulkanFramebuffer->width(), vulkanFramebuffer->height()};

    const auto& clearValues = vulkanRenderPass->clear_values();
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::end_render_pass() {
    vkCmdEndRenderPass(m_currentCommandBuffer);
    m_currentPipelineLayout = VK_NULL_HANDLE;
}

void VulkanCommandBuffer::bind_graphics_pipeline(GraphicsPipeline* pipeline) {
    auto vulkanPipeline = dynamic_cast<VulkanGraphicsPipeline*>(pipeline);

    auto imageIndex = *m_currentImagePtr;

    vkCmdBindPipeline(m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->handle(*m_currentImagePtr));

    m_currentPipelineLayout = vulkanPipeline->pipeline_layout();
    m_currentPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void VulkanCommandBuffer::bind_compute_pipeline(ComputePipeline* pipeline) {
    auto vulkanPipeline = dynamic_cast<VulkanComputePipeline*>(pipeline);

    vkCmdBindPipeline(m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkanPipeline->handle(*m_currentImagePtr));

    m_currentPipelineLayout = vulkanPipeline->pipeline_layout();
    m_currentPipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
}

void VulkanCommandBuffer::bind_vertex_buffer(Buffer** buffers, uint32_t bufferCount) {
    static constexpr auto kMaxBufferCount = 32;
    DUK_ASSERT(bufferCount < kMaxBufferCount);

    duk::tools::FixedVector<VkBuffer, kMaxBufferCount> bufferHandles;
    duk::tools::FixedVector<VkDeviceSize, kMaxBufferCount> offsets(bufferCount, 0);

    for (int i = 0; i < bufferCount; i++) {
        auto vulkanBuffer = dynamic_cast<VulkanBuffer*>(buffers[i]);
        VkBuffer handle = VK_NULL_HANDLE;
        if (vulkanBuffer) {
            handle = vulkanBuffer->handle(*m_currentImagePtr);
        }
        bufferHandles.push_back(handle);
    }

    vkCmdBindVertexBuffers(m_currentCommandBuffer, 0, bufferHandles.size(), bufferHandles.data(), offsets.data());
}

void VulkanCommandBuffer::bind_index_buffer(Buffer* buffer) {
    DUK_ASSERT(buffer->type() == Buffer::Type::INDEX_16 || buffer->type() == Buffer::Type::INDEX_32);

    auto currentImage = *m_currentImagePtr;
    auto indexType = buffer->type() == Buffer::Type::INDEX_16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;

    auto vulkanBuffer = dynamic_cast<VulkanBuffer*>(buffer);

    vkCmdBindIndexBuffer(m_currentCommandBuffer, vulkanBuffer->handle(currentImage), 0, indexType);
}

void VulkanCommandBuffer::bind_descriptor_set(DescriptorSet* descriptorSet, uint32_t setIndex) {
    DUK_ASSERT(m_currentPipelineLayout != VK_NULL_HANDLE);

    auto currentImage = *m_currentImagePtr;
    auto vulkanDescriptorSet = dynamic_cast<VulkanDescriptorSet*>(descriptorSet);

    auto handle = vulkanDescriptorSet->handle(currentImage);

    vkCmdBindDescriptorSets(m_currentCommandBuffer, m_currentPipelineBindPoint, m_currentPipelineLayout, setIndex, 1, &handle, 0, nullptr);
}

void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance) {
    vkCmdDraw(m_currentCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    vkCmdDrawIndexed(m_currentCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    vkCmdDispatch(m_currentCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::pipeline_barrier(const CommandBuffer::PipelineBarrier& barrier) {
    auto imageIndex = *m_currentImagePtr;

    for (int i = 0; i < barrier.bufferMemoryBarrierCount; i++) {
        VkBufferMemoryBarrier vkBufferMemoryBarrier = {};
        auto& dukBufferMemoryBarrier = barrier.bufferMemoryBarriers[i];
        vkBufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        vkBufferMemoryBarrier.size = dukBufferMemoryBarrier.size;
        vkBufferMemoryBarrier.offset = dukBufferMemoryBarrier.offset;
        vkBufferMemoryBarrier.buffer = dynamic_cast<VulkanBuffer*>(dukBufferMemoryBarrier.buffer)->handle(imageIndex);
        vkBufferMemoryBarrier.srcAccessMask = convert_access_mask(dukBufferMemoryBarrier.srcAccessMask);
        vkBufferMemoryBarrier.dstAccessMask = convert_access_mask(dukBufferMemoryBarrier.dstAccessMask);
        vkBufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        if (dukBufferMemoryBarrier.srcCommandQueue && dukBufferMemoryBarrier.dstCommandQueue) {
            auto srcQueueFamilyIndex = dynamic_cast<VulkanCommandQueue*>(dukBufferMemoryBarrier.srcCommandQueue)->family_index();
            auto dstQueueFamilyIndex = dynamic_cast<VulkanCommandQueue*>(dukBufferMemoryBarrier.dstCommandQueue)->family_index();
            if (srcQueueFamilyIndex != dstQueueFamilyIndex) {
                vkBufferMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
                vkBufferMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
            }
        }

        vkCmdPipelineBarrier(m_currentCommandBuffer, convert_pipeline_stage_mask(dukBufferMemoryBarrier.srcStageMask), convert_pipeline_stage_mask(dukBufferMemoryBarrier.dstStageMask), 0, 0, nullptr, 1, &vkBufferMemoryBarrier, 0, nullptr);
    }

    for (int i = 0; i < barrier.imageMemoryBarrierCount; i++) {
        VkImageMemoryBarrier vkImageMemoryBarrier = {};
        auto& dukImageMemoryBarrier = barrier.imageMemoryBarriers[i];
        auto vulkanImage = dynamic_cast<VulkanImage*>(dukImageMemoryBarrier.image);
        vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        vkImageMemoryBarrier.image = vulkanImage->image(imageIndex);
        vkImageMemoryBarrier.oldLayout = convert_layout(dukImageMemoryBarrier.oldLayout);
        vkImageMemoryBarrier.newLayout = convert_layout(dukImageMemoryBarrier.newLayout);
        vkImageMemoryBarrier.subresourceRange.aspectMask = vulkanImage->image_aspect();
        vkImageMemoryBarrier.subresourceRange.baseArrayLayer = dukImageMemoryBarrier.subresourceRange.baseArrayLayer;
        vkImageMemoryBarrier.subresourceRange.layerCount = dukImageMemoryBarrier.subresourceRange.layerCount;
        vkImageMemoryBarrier.subresourceRange.baseMipLevel = dukImageMemoryBarrier.subresourceRange.baseMipLevel;
        vkImageMemoryBarrier.subresourceRange.levelCount = dukImageMemoryBarrier.subresourceRange.levelCount;
        vkImageMemoryBarrier.srcAccessMask = convert_access_mask(dukImageMemoryBarrier.srcAccessMask);
        vkImageMemoryBarrier.dstAccessMask = convert_access_mask(dukImageMemoryBarrier.dstAccessMask);
        vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        if (dukImageMemoryBarrier.srcCommandQueue && dukImageMemoryBarrier.dstCommandQueue) {
            auto srcQueueFamilyIndex = dynamic_cast<VulkanCommandQueue*>(dukImageMemoryBarrier.srcCommandQueue)->family_index();
            auto dstQueueFamilyIndex = dynamic_cast<VulkanCommandQueue*>(dukImageMemoryBarrier.dstCommandQueue)->family_index();
            if (srcQueueFamilyIndex != dstQueueFamilyIndex) {
                vkImageMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
                vkImageMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
            }
        }

        vkCmdPipelineBarrier(m_currentCommandBuffer, convert_pipeline_stage_mask(dukImageMemoryBarrier.srcStageMask), convert_pipeline_stage_mask(dukImageMemoryBarrier.dstStageMask), 0, 0, nullptr, 0, nullptr, 1, &vkImageMemoryBarrier);
    }
}

Submitter* VulkanCommandBuffer::submitter_ptr() {
    return &m_submitter;
}

}// namespace duk::rhi
