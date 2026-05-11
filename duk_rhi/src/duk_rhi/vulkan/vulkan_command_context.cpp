/// vulkan_command_context.cpp

#include <duk_macros/assert.h>

#include <duk_rhi/vulkan/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_command_context.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_instance.h>
#include <duk_rhi/vulkan/vulkan_static_render_state.h>

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
    : m_instance(*createInfo.instance)
    , m_device(m_instance.device())
    , m_physicalDevice(*createInfo.physicalDevice)
    , m_queue(createInfo.queue)
    , m_framesInFlight(createInfo.framesInFlight)
    , m_swapchain(std::move(swapchain))
    , m_deletionQueue(m_framesInFlight) {

    // -----------------------------------------------------------------------
    // Descriptor layout cache and sampler cache
    // -----------------------------------------------------------------------
    VulkanDescriptorSetLayoutCacheCreateInfo descriptorSetLayoutCacheCreateInfo = {};
    descriptorSetLayoutCacheCreateInfo.device = m_device;
    m_descriptorSetLayoutCache = std::make_unique<VulkanDescriptorSetLayoutCache>(descriptorSetLayoutCacheCreateInfo);

    VulkanSamplerCacheCreateInfo samplerCacheCreateInfo = {};
    samplerCacheCreateInfo.device = m_device;
    m_samplerCache = std::make_unique<VulkanSamplerCache>(samplerCacheCreateInfo);

    VulkanStaticRenderStateCreateInfo renderStateCreateInfo = {};
    renderStateCreateInfo.device = m_device;
    renderStateCreateInfo.deletionQueue = &m_deletionQueue;
    m_renderState = std::make_unique<VulkanStaticRenderState>(renderStateCreateInfo);

    // -----------------------------------------------------------------------
    // Per-frame fences
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
    // Semaphore creation, if a swapchain is present we create one render semaphore per swapchain image
    // otherwise we have only one per frame in flight
    // -----------------------------------------------------------------------
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (m_swapchain) {
        m_imageSemaphores.resize(m_framesInFlight);
        for (auto& semaphore : m_imageSemaphores) {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
                throw std::runtime_error("VulkanCommandContext: failed to create image semaphore");
            }
        }

        m_renderSemaphores.resize(m_swapchain->image_count());
        for (auto& semaphore : m_renderSemaphores) {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
                throw std::runtime_error("VulkanCommandContext: failed to create render semaphore");
            }
        }
        m_defaultFrameBuffer = std::make_unique<VulkanFrameBuffer>();
    }
    else {
        m_renderSemaphores.resize(m_framesInFlight);
        for (auto& semaphore : m_renderSemaphores) {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
                throw std::runtime_error("VulkanCommandContext: failed to create render semaphore");
            }
        }
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
    for (auto semaphore : m_imageSemaphores) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    for (auto semaphore : m_renderSemaphores) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    for (auto fence : m_fences) {
        vkDestroyFence(m_device, fence, nullptr);
    }
}

void VulkanCommandContext::update() {
    m_frameIndex = (++m_frameCounter) % m_framesInFlight;

    // CPU-GPU sync: wait for the GPU to finish with this frame slot's previous work
    vkWaitForFences(m_device, 1, &m_fences[m_frameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());

    // The GPU has finished with resources submitted in this frame slot.
    // It is now safe to destroy anything that was queued for deletion.
    m_deletionQueue.flush(m_frameCounter);

    if (m_swapchain != nullptr) {
        const auto semaphore = m_imageSemaphores[m_frameIndex];
        auto result = m_swapchain->acquire_next_image(semaphore);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            auto deviceLock = m_instance.unique_device_lock();
            result = m_instance.wait_idle();
            if (result != VK_SUCCESS) {
                throw std::runtime_error("VulkanCommandContext: failed to wait for device idle on swapchain recreation");
            }
            m_swapchain->recreate();
            result = m_swapchain->acquire_next_image(semaphore);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("VulkanCommandContext: failed to acquire next image");
            }
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("VulkanCommandContext: failed to acquire next swapchain image");
        }
        m_swapchainImageIndex = m_swapchain->image_index();
    }
}

void VulkanCommandContext::flush() {
    if (m_activeCommandBuffer == VK_NULL_HANDLE) {
        return;
    }

    // -----------------------------------------------------------------------
    // Build wait/signal semaphore lists.
    // -----------------------------------------------------------------------
    tools::FixedVector<VkSemaphore, 1> waitSemaphores;
    tools::FixedVector<VkPipelineStageFlags, 1> waitStages;
    tools::FixedVector<VkSemaphore, 1> signalSemaphores;

    if (m_swapchain) {
        // wait for swapchain image
        waitSemaphores.push_back(m_imageSemaphores[m_frameIndex]);
        waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        // if rendering to the swapchain, use the swapchain indexed semaphore
        signalSemaphores.push_back(m_renderSemaphores[m_swapchainImageIndex]);

        // Transition current swapchain image to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR.
        // No-op if it is already there (e.g. compute-only or empty frame).
        m_swapchain->image(m_swapchainImageIndex)->transition_to(
            m_activeCommandBuffer,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    }
    else {
        // if not rendering to swapchain, just use a regular frame protected semaphore
        signalSemaphores.push_back(m_renderSemaphores[m_frameIndex]);
    }

    // end command buffer after layout transition, if any
    vkEndCommandBuffer(m_activeCommandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_activeCommandBuffer;
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    // Reset the fence so it can be signalled by this submit
    vkResetFences(m_device, 1, &m_fences[m_frameIndex]);
    m_queue->submit(1, &submitInfo, m_fences[m_frameIndex]);

    if (m_swapchain != nullptr) {
        auto result = m_swapchain->present(*m_queue, signalSemaphores[0]);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            auto deviceLock = m_instance.unique_device_lock();
            result = m_instance.wait_idle();
            if (result != VK_SUCCESS) {
                throw std::runtime_error("VulkanCommandContext: failed to wait for device idle on swapchain recreation after present");
            }
            m_swapchain->recreate();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("VulkanCommandContext: failed to present swapchain image");
        }
    }
    m_activeCommandBuffer = VK_NULL_HANDLE;
}

void VulkanCommandContext::render_begin(const RenderBeginParams& params) {
    const auto commandBuffer = current_command_buffer();
    auto frameBuffer = static_cast<const VulkanFrameBuffer*>(params.frameBuffer);
    if (!frameBuffer) {
        if (!m_swapchain) {
            throw std::runtime_error("VulkanCommandContext: no framebuffer provided to offscreen context");
        }
        const VulkanImage* attachments[1] = {m_swapchain->image(m_swapchainImageIndex)};
        m_defaultFrameBuffer->write(attachments, 1);
        frameBuffer = m_defaultFrameBuffer.get();
    }
    m_renderState->begin(commandBuffer, *frameBuffer, convert_load_op(params.loadOp), convert_store_op(params.storeOp), params.clearColor);
}

void VulkanCommandContext::bind_render_shader(const BindShaderParams& params, const PipelineState& pipelineState) {
    const auto commandBuffer = current_command_buffer();
    const auto shader = static_cast<const VulkanShader*>(params.shader);
    m_renderState->bind_pipeline(commandBuffer, *shader, pipelineState);
}

void VulkanCommandContext::bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) {
    const auto commandBuffer = current_command_buffer();
    const auto vertexBuffer = reinterpret_cast<const VulkanBuffer* const*>(vertexBuffers);
    std::array<VkBuffer, 16> bufferHandles;
    std::array<VkDeviceSize, 16> bufferOffsets;
    for (uint32_t i = 0; i < count; ++i) {
        auto buffer = vertexBuffer[i];
        if (buffer) {
            bufferHandles[i] = buffer->handle();
        }
        else {
            bufferHandles[i] = VK_NULL_HANDLE;
        }
        bufferOffsets[i] = 0;
    }
    vkCmdBindVertexBuffers(commandBuffer, 0, count, bufferHandles.data(), bufferOffsets.data());
}

void VulkanCommandContext::bind_index_buffer(const Buffer* buffer) {
    const auto commandBuffer = current_command_buffer();
    const auto vulkanBuffer = static_cast<const VulkanBuffer*>(buffer);
    DUK_ASSERT(vulkanBuffer->index_type() != VK_INDEX_TYPE_MAX_ENUM);
    vkCmdBindIndexBuffer(commandBuffer, vulkanBuffer->handle(), 0, vulkanBuffer->index_type());
}

void VulkanCommandContext::render(const RenderParams& params) {
    const auto commandBuffer = current_command_buffer();
    vkCmdDraw(commandBuffer, params.vertexCount, params.instanceCount, params.firstVertex, params.firstInstance);
}

void VulkanCommandContext::render_indirect(const std::span<const RenderParams>& indirectParams) {

}

void VulkanCommandContext::render_indexed(const RenderIndexedParams& params) {
    const auto commandBuffer = current_command_buffer();
    vkCmdDrawIndexed(commandBuffer, params.indexCount, params.instanceCount, params.firstIndex, params.vertexOffset, params.firstInstance);
}

void VulkanCommandContext::render_indexed_indirect(const std::span<const RenderIndexedParams>& indexedIndirectParams) {
}

void VulkanCommandContext::render_end() {
    const auto commandBuffer = current_command_buffer();
    m_renderState->end(commandBuffer);
}

void VulkanCommandContext::bind_compute_shader(const BindShaderParams& params) {
}

void VulkanCommandContext::compute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
}

VkCommandBuffer VulkanCommandContext::current_command_buffer() {
    if (m_activeCommandBuffer == VK_NULL_HANDLE) {
        m_activeCommandBuffer = m_commandBuffers[m_frameIndex];

        vkResetCommandBuffer(m_activeCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(m_activeCommandBuffer, &beginInfo);
    }
    return m_activeCommandBuffer;
}

std::shared_ptr<Shader> VulkanCommandContext::create_shader(const ShaderCreateInfo& shaderCreateInfo) {
    VulkanShaderCreateInfo vulkanShaderCreateInfo = {};
    vulkanShaderCreateInfo.shaderDataSource = shaderCreateInfo.shaderDataSource;
    vulkanShaderCreateInfo.device = m_device;
    vulkanShaderCreateInfo.descriptorSetLayoutCache = m_descriptorSetLayoutCache.get();
    return make_managed<VulkanShader>(vulkanShaderCreateInfo);
}

std::shared_ptr<Buffer> VulkanCommandContext::create_buffer(const BufferCreateInfo& bufferCreateInfo) {
    VulkanBufferCreateInfo vulkanBufferCreateInfo = {};
    vulkanBufferCreateInfo.usageFlags = detail::buffer_usage_flags(bufferCreateInfo.type);
    vulkanBufferCreateInfo.memoryFlags = detail::buffer_memory_flags(bufferCreateInfo.updateFrequency);
    vulkanBufferCreateInfo.indexType = detail::buffer_index_type(bufferCreateInfo.type);
    vulkanBufferCreateInfo.device = m_device;
    vulkanBufferCreateInfo.physicalDevice = &m_physicalDevice;
    return make_managed<VulkanBuffer>(vulkanBufferCreateInfo);
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
        stagingCreateInfo.physicalDevice = &m_physicalDevice;
        stagingCreateInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        stagingCreateInfo.size = size;

        auto staging = std::make_unique<VulkanBuffer>(stagingCreateInfo);
        staging->write(src, size, 0);

        VkBufferCopy region = {};
        region.srcOffset = 0;
        region.dstOffset = offset;
        region.size = size;
        vkCmdCopyBuffer(current_command_buffer(), staging->handle(), vulkanBuffer->handle(), 1, &region);

        // Keep the staging buffer alive until the GPU has finished using it.
        m_deletionQueue.push(staging.release(), m_frameCounter);
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
    vulkanMemoryImageCreateInfo.physicalDevice = &m_physicalDevice;
    vulkanMemoryImageCreateInfo.format = convert_pixel_format(imageCreateInfo.format);
    vulkanMemoryImageCreateInfo.width = imageCreateInfo.width;
    vulkanMemoryImageCreateInfo.height = imageCreateInfo.height;
    vulkanMemoryImageCreateInfo.usageFlags = detail::image_usage_flags(imageCreateInfo.usage);
    vulkanMemoryImageCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vulkanMemoryImageCreateInfo.aspectFlags = detail::image_aspect_flags(imageCreateInfo.usage, imageCreateInfo.format);
    return make_managed<VulkanImage>(vulkanMemoryImageCreateInfo);
}

void VulkanCommandContext::write_image(Image* image, const void* src, size_t size) {
    auto* vulkanImage = static_cast<VulkanImage*>(image);

    VulkanBufferCreateInfo stagingCi = {};
    stagingCi.device = m_device;
    stagingCi.physicalDevice = &m_physicalDevice;
    stagingCi.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingCi.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    stagingCi.size = size;

    auto staging = std::make_unique<VulkanBuffer>(stagingCi);
    staging->write(src, size, 0);

    auto commandBuffer = current_command_buffer();

    // Transition the image into the transfer-destination layout before writing.
    vulkanImage->transition_to(commandBuffer,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                              VK_PIPELINE_STAGE_TRANSFER_BIT);

    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = vulkanImage->image_aspect();
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {vulkanImage->width(), vulkanImage->height(), 1};

    vkCmdCopyBufferToImage(commandBuffer, staging->handle(), vulkanImage->image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    m_deletionQueue.push(staging.release(), m_frameCounter);
}

std::shared_ptr<FrameBuffer> VulkanCommandContext::create_frame_buffer() {
    return make_managed<VulkanFrameBuffer>();
}

void VulkanCommandContext::write_frame_buffer(FrameBuffer* frameBuffer, const Image* const* attachments, uint32_t attachmentCount) {
    auto* vulkanFrameBuffer = static_cast<VulkanFrameBuffer*>(frameBuffer);

    vulkanFrameBuffer->write(
        reinterpret_cast<const VulkanImage* const*>(attachments),
        attachmentCount
    );
}

}// namespace duk::rhi


