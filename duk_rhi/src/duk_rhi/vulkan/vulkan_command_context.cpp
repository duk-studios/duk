/// vulkan_command_context.cpp

#include <duk_macros/assert.h>

#include <duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_pipeline_flags.h>
#include <duk_rhi/vulkan/pipeline/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_command_context.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_rhi/image_data_source.h>

#include <duk_tools/fixed_vector.h>

#include <limits>
#include <stdexcept>
#include <vector>

namespace duk::rhi {

namespace detail {

static VkBufferUsageFlags buffer_usage_flags(Buffer::Type type) {
    VkBufferUsageFlags flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    switch (type) {
        case Buffer::Type::INDEX_16:
        case Buffer::Type::INDEX_32:  flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;   break;
        case Buffer::Type::VERTEX:    flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;  break;
        case Buffer::Type::UNIFORM:   flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
        case Buffer::Type::STORAGE:   flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; break;
        case Buffer::Type::INDIRECT:  flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT; break;
        default: throw std::invalid_argument("unhandled Buffer::Type");
    }
    return flags;
}

static VkMemoryPropertyFlags buffer_memory_flags(Buffer::UpdateFrequency freq) {
    if (freq == Buffer::UpdateFrequency::STATIC) {
        return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
}

static VkIndexType buffer_index_type(Buffer::Type type) {
    switch (type) {
        case Buffer::Type::INDEX_16: return VK_INDEX_TYPE_UINT16;
        case Buffer::Type::INDEX_32: return VK_INDEX_TYPE_UINT32;
        default:                     return VK_INDEX_TYPE_MAX_ENUM;
    }
}

static VkImageUsageFlags image_usage_flags(Image::Usage usage) {
    // Always include TRANSFER_DST so write_image() can be called on any image.
    return convert_usage(usage) | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
}

static VkImageAspectFlags image_aspect_flags(Image::Usage usage, PixelFormat format) {
    switch (usage) {
        case Image::Usage::COLOR_ATTACHMENT:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case Image::Usage::SAMPLED:
        case Image::Usage::STORAGE:
        case Image::Usage::SAMPLED_STORAGE:
        case Image::Usage::DEPTH_STENCIL_ATTACHMENT:
            if (format.is_depth()) {
                VkImageAspectFlags flags = VK_IMAGE_ASPECT_DEPTH_BIT;
                if (format.is_stencil()) {
                    flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
                return flags;
            }
            return VK_IMAGE_ASPECT_COLOR_BIT;
        default:
            throw std::invalid_argument("unhandled Image::Usage for aspect flags");
    }
}

}// namespace detail

VulkanCommandContext::VulkanCommandContext(const VulkanCommandContextCreateInfo& createInfo, std::unique_ptr<VulkanSwapchain> swapchain)
    : m_device(createInfo.device)
    , m_physicalDevice(createInfo.physicalDevice)
    , m_queue(createInfo.queue)
    , m_framesInFlight(createInfo.framesInFlight)
    , m_swapchain(std::move(swapchain))
    , m_deletionQueue(std::make_shared<VulkanDeletionQueue>()) {

    // -----------------------------------------------------------------------
    // Descriptor layout cache and sampler cache
    // -----------------------------------------------------------------------
    VulkanDescriptorSetLayoutCacheCreateInfo descriptorSetLayoutCacheCreateInfo = {};
    descriptorSetLayoutCacheCreateInfo.device = m_device;
    m_descriptorSetLayoutCache = std::make_unique<VulkanDescriptorSetLayoutCache>(descriptorSetLayoutCacheCreateInfo);

    VulkanSamplerCacheCreateInfo samplerCacheCreateInfo = {};
    samplerCacheCreateInfo.device = m_device;
    m_samplerCache = std::make_unique<VulkanSamplerCache>(samplerCacheCreateInfo);

    // -----------------------------------------------------------------------
    // Per-frame fences (pre-signalled so the first update() returns immediately)
    // -----------------------------------------------------------------------
    m_fences.resize(m_framesInFlight);
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (auto& fence : m_fences) {
        if (vkCreateFence(m_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
            throw std::runtime_error("VulkanCommandContext: failed to create per-frame fence");
        }
    }

    // -----------------------------------------------------------------------
    // Single timeline semaphore (VK_KHR_timeline_semaphore)
    // -----------------------------------------------------------------------
    VkSemaphoreTypeCreateInfoKHR semaphoreTypeInfo = {};
    semaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR;
    semaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE_KHR;
    semaphoreTypeInfo.initialValue = 0;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = &semaphoreTypeInfo;

    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_timelineSemaphore) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCommandContext: failed to create timeline semaphore");
    }

    // -----------------------------------------------------------------------
    // Command pool and per-frame command buffers
    // -----------------------------------------------------------------------
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_queue->family_index();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCommandContext: failed to create command pool");
    }

    m_commandBuffers.resize(m_framesInFlight);
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = m_framesInFlight;

    if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCommandContext: failed to allocate command buffers");
    }
}

VulkanCommandContext::~VulkanCommandContext() {
    if (m_commandPool != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }
    if (m_timelineSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_device, m_timelineSemaphore, nullptr);
    }
    for (auto fence : m_fences) {
        vkDestroyFence(m_device, fence, nullptr);
    }
}

void VulkanCommandContext::update() {
    m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;

    // CPU-GPU sync: wait for the GPU to finish with this frame slot's previous work
    vkWaitForFences(m_device, 1, &m_fences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    // The GPU has finished with resources submitted in this frame slot.
    // It is now safe to destroy anything that was queued for deletion.
    m_deletionQueue->flush();

    if (m_swapchain != nullptr) {
        m_currentImage = m_swapchain->acquire_next_image(m_currentFrame);
        auto& frameSync = m_swapchain->frame_sync(m_currentFrame);
        m_imageAvailableSemaphore = frameSync.imageAvailableSemaphore;
        m_renderFinishedSemaphore = frameSync.renderFinishedSemaphore;
        m_swapchainImageAcquired = true;
    }
}

void VulkanCommandContext::flush() {
    if (m_activeCommandBuffer == VK_NULL_HANDLE) {
        return;
    }

    vkEndCommandBuffer(m_activeCommandBuffer);

    // -----------------------------------------------------------------------
    // Build wait/signal semaphore lists.
    // -----------------------------------------------------------------------
    duk::tools::FixedVector<VkSemaphore, 2> waitSemaphores;
    duk::tools::FixedVector<VkPipelineStageFlags, 2> waitStages;
    duk::tools::FixedVector<uint64_t, 2> waitValues;

    duk::tools::FixedVector<VkSemaphore, 2> signalSemaphores;
    duk::tools::FixedVector<uint64_t, 2> signalValues;

    if (m_swapchainImageAcquired) {
        // Binary semaphore: wait for the swapchain image to become available
        waitSemaphores.push_back(m_imageAvailableSemaphore);
        waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        waitValues.push_back(0); // ignored for binary semaphores

        // Binary semaphore: signal when rendering is done so present can proceed
        signalSemaphores.push_back(m_renderFinishedSemaphore);
        signalValues.push_back(0); // ignored for binary semaphores
    }

    // Timeline semaphore: always signal with the next counter value
    const auto nextTimelineValue = m_timelineValue + 1;
    signalSemaphores.push_back(m_timelineSemaphore);
    signalValues.push_back(nextTimelineValue);

    VkTimelineSemaphoreSubmitInfoKHR timelineInfo = {};
    timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR;
    timelineInfo.waitSemaphoreValueCount = static_cast<uint32_t>(waitValues.size());
    timelineInfo.pWaitSemaphoreValues = waitValues.data();
    timelineInfo.signalSemaphoreValueCount = static_cast<uint32_t>(signalValues.size());
    timelineInfo.pSignalSemaphoreValues = signalValues.data();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = &timelineInfo;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_activeCommandBuffer;
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    // Reset the fence so it can be signalled by this submit
    vkResetFences(m_device, 1, &m_fences[m_currentFrame]);
    m_queue->submit(1, &submitInfo, m_fences[m_currentFrame]);

    if (m_swapchain != nullptr) {
        m_swapchain->present(m_currentImage, m_currentFrame);
    }

    m_timelineValue = nextTimelineValue;
    m_activeCommandBuffer = VK_NULL_HANDLE;
    m_swapchainImageAcquired = false;
    m_currentPipelineLayout = VK_NULL_HANDLE;
}

VkCommandBuffer VulkanCommandContext::current_command_buffer() {
    if (m_activeCommandBuffer == VK_NULL_HANDLE) {
        m_activeCommandBuffer = m_commandBuffers[m_currentFrame];
        m_currentPipelineLayout = VK_NULL_HANDLE;
        m_currentPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        vkResetCommandBuffer(m_activeCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(m_activeCommandBuffer, &beginInfo);
    }
    return m_activeCommandBuffer;
}

void VulkanCommandContext::begin_render_pass(RenderPass* renderPass, FrameBuffer* frameBuffer) {
    auto commandBuffer = current_command_buffer();

    auto vulkanFramebuffer = static_cast<VulkanFrameBuffer*>(frameBuffer);
    auto vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->handle();
    renderPassInfo.framebuffer = vulkanFramebuffer->handle();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {vulkanFramebuffer->width(), vulkanFramebuffer->height()};

    const auto& clearValues = vulkanRenderPass->clear_values();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandContext::end_render_pass() {
    vkCmdEndRenderPass(current_command_buffer());
    m_currentPipelineLayout = VK_NULL_HANDLE;
}

void VulkanCommandContext::bind_graphics_pipeline(GraphicsPipeline* pipeline) {
    auto commandBuffer = current_command_buffer();
    auto vulkanPipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->handle());
    m_currentPipelineLayout = vulkanPipeline->pipeline_layout();
    m_currentPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void VulkanCommandContext::bind_compute_pipeline(ComputePipeline* pipeline) {
    auto commandBuffer = current_command_buffer();
    auto vulkanPipeline = static_cast<VulkanComputePipeline*>(pipeline);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkanPipeline->handle());
    m_currentPipelineLayout = vulkanPipeline->pipeline_layout();
    m_currentPipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
}

void VulkanCommandContext::bind_vertex_buffer(const Buffer** buffers, uint32_t bufferCount, uint32_t firstBinding) {
    auto commandBuffer = current_command_buffer();
    static constexpr auto kMaxBufferCount = 32;
    DUK_ASSERT(bufferCount + firstBinding < kMaxBufferCount);

    duk::tools::FixedVector<VkBuffer, kMaxBufferCount> bufferHandles;
    duk::tools::FixedVector<VkDeviceSize, kMaxBufferCount> offsets(bufferCount, 0);

    for (uint32_t i = 0; i < bufferCount; i++) {
        auto vulkanBuffer = static_cast<const VulkanBuffer*>(buffers[i]);
        bufferHandles.push_back(vulkanBuffer ? vulkanBuffer->handle() : VK_NULL_HANDLE);
    }

    vkCmdBindVertexBuffers(commandBuffer, firstBinding, bufferHandles.size(), bufferHandles.data(), offsets.data());
}

void VulkanCommandContext::bind_index_buffer(const Buffer* buffer) {
    auto commandBuffer = current_command_buffer();
    auto vulkanBuffer = static_cast<const VulkanBuffer*>(buffer);
    DUK_ASSERT(vulkanBuffer->index_type() != VK_INDEX_TYPE_MAX_ENUM);
    vkCmdBindIndexBuffer(commandBuffer, vulkanBuffer->handle(), 0, vulkanBuffer->index_type());
}

void VulkanCommandContext::bind_descriptor_set(DescriptorSet* descriptorSet, uint32_t setIndex) {
    auto commandBuffer = current_command_buffer();
    DUK_ASSERT(m_currentPipelineLayout != VK_NULL_HANDLE);
    auto vulkanDescriptorSet = static_cast<VulkanDescriptorSet*>(descriptorSet);
    auto handle = vulkanDescriptorSet->handle();
    vkCmdBindDescriptorSets(commandBuffer, m_currentPipelineBindPoint, m_currentPipelineLayout, setIndex, 1, &handle, 0, nullptr);
}

void VulkanCommandContext::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    vkCmdDraw(current_command_buffer(), vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandContext::draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    vkCmdDrawIndexed(current_command_buffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandContext::draw_indirect(const Buffer* buffer, size_t offset, uint32_t drawCount) {
    auto bufferHandle = static_cast<const VulkanBuffer*>(buffer)->handle();
    vkCmdDrawIndirect(current_command_buffer(), bufferHandle, offset, drawCount, sizeof(VkDrawIndirectCommand));
}

void VulkanCommandContext::draw_indirect_indexed(const Buffer* buffer, size_t offset, uint32_t drawCount) {
    auto bufferHandle = static_cast<const VulkanBuffer*>(buffer)->handle();
    vkCmdDrawIndexedIndirect(current_command_buffer(), bufferHandle, offset, drawCount, sizeof(VkDrawIndexedIndirectCommand));
}

void VulkanCommandContext::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    vkCmdDispatch(current_command_buffer(), groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandContext::pipeline_barrier(const PipelineBarrier& barrier) {
    auto commandBuffer = current_command_buffer();

    for (uint32_t i = 0; i < barrier.bufferMemoryBarrierCount; i++) {
        const auto& b = barrier.bufferMemoryBarriers[i];
        VkBufferMemoryBarrier vkBarrier = {};
        vkBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        vkBarrier.size = b.size;
        vkBarrier.offset = b.offset;
        vkBarrier.buffer = static_cast<VulkanBuffer*>(b.buffer)->handle();
        vkBarrier.srcAccessMask = convert_access_mask(b.srcAccessMask);
        vkBarrier.dstAccessMask = convert_access_mask(b.dstAccessMask);
        vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        if (b.srcCommandQueue && b.dstCommandQueue) {
            auto srcFamily = static_cast<VulkanCommandQueue*>(b.srcCommandQueue)->family_index();
            auto dstFamily = static_cast<VulkanCommandQueue*>(b.dstCommandQueue)->family_index();
            if (srcFamily != dstFamily) {
                vkBarrier.srcQueueFamilyIndex = srcFamily;
                vkBarrier.dstQueueFamilyIndex = dstFamily;
            }
        }
        vkCmdPipelineBarrier(commandBuffer,
                convert_pipeline_stage_mask(b.srcStageMask), convert_pipeline_stage_mask(b.dstStageMask),
                0, 0, nullptr, 1, &vkBarrier, 0, nullptr);
    }

    for (uint32_t i = 0; i < barrier.imageMemoryBarrierCount; i++) {
        const auto& b = barrier.imageMemoryBarriers[i];
        auto vulkanImage = static_cast<VulkanImage*>(b.image);
        VkImageMemoryBarrier vkBarrier = {};
        vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        vkBarrier.image = vulkanImage->image();
        vkBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        vkBarrier.subresourceRange.aspectMask = vulkanImage->image_aspect();
        vkBarrier.subresourceRange.baseArrayLayer = b.subresourceRange.baseArrayLayer;
        vkBarrier.subresourceRange.layerCount = b.subresourceRange.layerCount;
        vkBarrier.subresourceRange.baseMipLevel = b.subresourceRange.baseMipLevel;
        vkBarrier.subresourceRange.levelCount = b.subresourceRange.levelCount;
        vkBarrier.srcAccessMask = convert_access_mask(b.srcAccessMask);
        vkBarrier.dstAccessMask = convert_access_mask(b.dstAccessMask);
        vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        if (b.srcCommandQueue && b.dstCommandQueue) {
            auto srcFamily = static_cast<VulkanCommandQueue*>(b.srcCommandQueue)->family_index();
            auto dstFamily = static_cast<VulkanCommandQueue*>(b.dstCommandQueue)->family_index();
            if (srcFamily != dstFamily) {
                vkBarrier.srcQueueFamilyIndex = srcFamily;
                vkBarrier.dstQueueFamilyIndex = dstFamily;
            }
        }
        vkCmdPipelineBarrier(commandBuffer,
                convert_pipeline_stage_mask(b.srcStageMask), convert_pipeline_stage_mask(b.dstStageMask),
                0, 0, nullptr, 0, nullptr, 1, &vkBarrier);
    }
}

std::shared_ptr<Shader> VulkanCommandContext::create_shader(const ShaderCreateInfo& shaderCreateInfo) {
    VulkanShaderCreateInfo vulkanShaderCreateInfo = {};
    vulkanShaderCreateInfo.shaderDataSource = shaderCreateInfo.shaderDataSource;
    vulkanShaderCreateInfo.device = m_device;
    vulkanShaderCreateInfo.descriptorSetLayoutCache = m_descriptorSetLayoutCache.get();
    return make_managed(new VulkanShader(vulkanShaderCreateInfo));
}

std::shared_ptr<GraphicsPipeline> VulkanCommandContext::create_graphics_pipeline(const GraphicsPipelineCreateInfo& pipelineCreateInfo) {
    VulkanGraphicsPipelineCreateInfo vulkanPipelineCreateInfo = {};
    vulkanPipelineCreateInfo.device = m_device;
    vulkanPipelineCreateInfo.shader = static_cast<VulkanShader*>(pipelineCreateInfo.shader);
    vulkanPipelineCreateInfo.renderPass = static_cast<VulkanRenderPass*>(pipelineCreateInfo.renderPass);
    vulkanPipelineCreateInfo.viewport = pipelineCreateInfo.viewport;
    vulkanPipelineCreateInfo.scissor = pipelineCreateInfo.scissor;
    vulkanPipelineCreateInfo.blend = pipelineCreateInfo.blend;
    vulkanPipelineCreateInfo.cullModeMask = pipelineCreateInfo.cullModeMask;
    vulkanPipelineCreateInfo.depthTesting = pipelineCreateInfo.depthTesting;
    vulkanPipelineCreateInfo.topology = pipelineCreateInfo.topology;
    vulkanPipelineCreateInfo.fillMode = pipelineCreateInfo.fillMode;
    return make_managed(new VulkanGraphicsPipeline(vulkanPipelineCreateInfo));
}

std::shared_ptr<ComputePipeline> VulkanCommandContext::create_compute_pipeline(const ComputePipelineCreateInfo& pipelineCreateInfo) {
    VulkanComputePipelineCreateInfo vulkanPipelineCreateInfo = {};
    vulkanPipelineCreateInfo.device = m_device;
    vulkanPipelineCreateInfo.shader = static_cast<VulkanShader*>(pipelineCreateInfo.shader);
    return make_managed(new VulkanComputePipeline(vulkanPipelineCreateInfo));
}

std::shared_ptr<RenderPass> VulkanCommandContext::create_render_pass(const RenderPassCreateInfo& renderPassCreateInfo) {
    VulkanRenderPassCreateInfo vulkanRenderPassCreateInfo = {};
    vulkanRenderPassCreateInfo.device = m_device;
    vulkanRenderPassCreateInfo.colorAttachments = renderPassCreateInfo.colorAttachments;
    vulkanRenderPassCreateInfo.colorAttachmentCount = renderPassCreateInfo.colorAttachmentCount;
    vulkanRenderPassCreateInfo.depthAttachment = renderPassCreateInfo.depthAttachment;
    return make_managed(new VulkanRenderPass(vulkanRenderPassCreateInfo));
}

std::shared_ptr<Buffer> VulkanCommandContext::create_buffer(const BufferCreateInfo& bufferCreateInfo) {
    VulkanBufferCreateInfo vulkanBufferCreateInfo = {};
    vulkanBufferCreateInfo.usageFlags = detail::buffer_usage_flags(bufferCreateInfo.type);
    vulkanBufferCreateInfo.memoryFlags = detail::buffer_memory_flags(bufferCreateInfo.updateFrequency);
    vulkanBufferCreateInfo.indexType = detail::buffer_index_type(bufferCreateInfo.type);
    vulkanBufferCreateInfo.device = m_device;
    vulkanBufferCreateInfo.physicalDevice = m_physicalDevice;
    return make_managed(new VulkanBuffer(vulkanBufferCreateInfo));
}

void VulkanCommandContext::write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) {
    auto* vulkanBuffer = static_cast<VulkanBuffer*>(buffer);

    if (vulkanBuffer->memory_flags() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        // DYNAMIC buffer: direct CPU map + copy, no command buffer needed.
        vulkanBuffer->write(src, size, offset);
    } else {
        // STATIC (device-local) buffer: upload via a temporary host-visible VulkanBuffer
        // recorded into the current command buffer as a transfer source.
        VulkanBufferCreateInfo stagingCreateInfo = {};
        stagingCreateInfo.device = m_device;
        stagingCreateInfo.physicalDevice = m_physicalDevice;
        stagingCreateInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        stagingCreateInfo.size = size;

        auto* staging = new VulkanBuffer(stagingCreateInfo);
        staging->write(src, size, 0);

        VkBufferCopy region = {};
        region.srcOffset = 0;
        region.dstOffset = offset;
        region.size = size;
        vkCmdCopyBuffer(current_command_buffer(), staging->handle(), vulkanBuffer->handle(), 1, &region);

        // Keep the staging buffer alive until the GPU has finished using it.
        m_deletionQueue->push(staging);
    }
}

void VulkanCommandContext::read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) {
    auto* vulkanBuffer = static_cast<VulkanBuffer*>(buffer);
    // Only host-visible (DYNAMIC) buffers support direct CPU reads.
    // STATIC (device-local) reads would require a read-back staging pass — not yet implemented.
    DUK_ASSERT(vulkanBuffer->memory_flags() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    vulkanBuffer->read(dst, size, offset);
}

std::shared_ptr<Image> VulkanCommandContext::create_image(const ImageCreateInfo& imageCreateInfo) {
    VulkanMemoryImageCreateInfo vulkanMemoryImageCreateInfo = {};
    vulkanMemoryImageCreateInfo.device = m_device;
    vulkanMemoryImageCreateInfo.physicalDevice = m_physicalDevice;
    vulkanMemoryImageCreateInfo.format = convert_pixel_format(imageCreateInfo.format);
    vulkanMemoryImageCreateInfo.width = imageCreateInfo.width;
    vulkanMemoryImageCreateInfo.height = imageCreateInfo.height;
    vulkanMemoryImageCreateInfo.usageFlags = detail::image_usage_flags(imageCreateInfo.usage);
    vulkanMemoryImageCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vulkanMemoryImageCreateInfo.aspectFlags = detail::image_aspect_flags(imageCreateInfo.usage, imageCreateInfo.format);
    return make_managed(new VulkanImage(vulkanMemoryImageCreateInfo));
}

void VulkanCommandContext::write_image(Image* image, const void* src, size_t size) {
    auto* vulkanImage = static_cast<VulkanImage*>(image);

    VulkanBufferCreateInfo stagingCi = {};
    stagingCi.device = m_device;
    stagingCi.physicalDevice = m_physicalDevice;
    stagingCi.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingCi.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    stagingCi.size = size;

    auto* staging = new VulkanBuffer(stagingCi);
    staging->write(src, size, 0);

    VkImageSubresourceRange subresource = {};
    subresource.aspectMask = vulkanImage->image_aspect();
    subresource.baseMipLevel = 0;
    subresource.levelCount = 1;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = 1;

    VulkanImage::CopyBufferToImageInfo copyInfo = {};
    copyInfo.buffer = staging->handle();
    copyInfo.image = vulkanImage->image();
    copyInfo.width = vulkanImage->width();
    copyInfo.height = vulkanImage->height();
    copyInfo.subresourceRange = subresource;

    VulkanImage::copy_buffer_to_image(current_command_buffer(), copyInfo);

    m_deletionQueue->push(staging);
}

void VulkanCommandContext::write_image(Image* image, const ImageDataSource* dataSource) {
    const size_t dataSize = dataSource->byte_count();
    std::vector<uint8_t> pixels(dataSize);
    dataSource->read_bytes(pixels.data(), dataSize, 0);
    write_image(image, pixels.data(), dataSize);
}

std::shared_ptr<DescriptorSet> VulkanCommandContext::create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetCreateInfo) {
    VulkanDescriptorSetCreateInfo vulkanDescriptorSetCreateInfo = {};
    vulkanDescriptorSetCreateInfo.device = m_device;
    vulkanDescriptorSetCreateInfo.descriptorSetDescription = descriptorSetCreateInfo.description;
    vulkanDescriptorSetCreateInfo.descriptorSetLayoutCache = m_descriptorSetLayoutCache.get();
    vulkanDescriptorSetCreateInfo.samplerCache = m_samplerCache.get();
    return make_managed(new VulkanDescriptorSet(vulkanDescriptorSetCreateInfo));
}

std::shared_ptr<FrameBuffer> VulkanCommandContext::create_frame_buffer(const FrameBufferCreateInfo& frameBufferCreateInfo) {
    VulkanFrameBufferCreateInfo vulkanFrameBufferCreateInfo = {};
    vulkanFrameBufferCreateInfo.device = m_device;
    vulkanFrameBufferCreateInfo.renderPass = static_cast<VulkanRenderPass*>(frameBufferCreateInfo.renderPass);
    vulkanFrameBufferCreateInfo.attachments = reinterpret_cast<VulkanImage**>(frameBufferCreateInfo.attachments);
    vulkanFrameBufferCreateInfo.attachmentCount = frameBufferCreateInfo.attachmentCount;
    vulkanFrameBufferCreateInfo.imageIndex = m_currentImage;
    return make_managed(new VulkanFrameBuffer(vulkanFrameBufferCreateInfo));
}

uint32_t VulkanCommandContext::current_frame() const {
    return m_currentFrame;
}

const uint32_t* VulkanCommandContext::current_frame_ptr() const {
    return &m_currentFrame;
}

uint32_t VulkanCommandContext::current_image() const {
    return m_currentImage;
}

const uint32_t* VulkanCommandContext::current_image_ptr() const {
    return &m_currentImage;
}

VkDevice VulkanCommandContext::device() const {
    return m_device;
}

VulkanPhysicalDevice* VulkanCommandContext::physical_device() const {
    return m_physicalDevice;
}

VulkanDescriptorSetLayoutCache* VulkanCommandContext::descriptor_set_layout_cache() const {
    return m_descriptorSetLayoutCache.get();
}

VulkanSamplerCache* VulkanCommandContext::sampler_cache() const {
    return m_samplerCache.get();
}

VkSemaphore VulkanCommandContext::timeline_semaphore() const {
    return m_timelineSemaphore;
}

uint64_t VulkanCommandContext::timeline_value() const {
    return m_timelineValue;
}

}// namespace duk::rhi


