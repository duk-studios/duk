/// 28/05/2023
/// vulkan_resource_manager.cpp

#include <duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_rhi/vulkan/vulkan_resource_manager.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>

namespace duk::rhi {

namespace detail {

template<typename T>
void create(std::vector<T*>& resources, uint32_t imageCount) {
    for (auto& resource: resources) {
        resource->create(imageCount);
    }
}

template<typename T>
void clean(std::vector<T*>& resources) {
    for (auto& resource: resources) {
        resource->clean();
    }
}

}// namespace detail

VulkanResourceManager::VulkanResourceManager(const VulkanResourceManagerCreateInfo& resourceManagerCreateInfo)
    : m_swapchain(resourceManagerCreateInfo.swapchain)
    , m_imageCount(resourceManagerCreateInfo.imageCount) {
    if (m_swapchain) {
        m_listener.listen(*m_swapchain->create_event(), [this](const auto& swapchainInfo) {
            m_imageCount = swapchainInfo.imageCount;
        });
    }

    m_listener.listen(*resourceManagerCreateInfo.prepareFrameEvent, [this](uint32_t frameIndex) {
        clean(m_swapchain ? *m_swapchain->current_image_ptr() : frameIndex);
    });
}

VulkanResourceManager::~VulkanResourceManager() {
    clean(m_commandQueues);
    clean(m_graphicsPipelines);
    clean(m_computePipelines);
    clean(m_descriptorSets);
    clean(m_frameBuffers);
    clean(m_renderPasses);
    clean(m_buffers);
    clean(m_images);
    DUK_ASSERT(m_renderPasses.all.empty());
    DUK_ASSERT(m_frameBuffers.all.empty());
    DUK_ASSERT(m_descriptorSets.all.empty());
    DUK_ASSERT(m_images.all.empty());
    DUK_ASSERT(m_buffers.all.empty());
    DUK_ASSERT(m_graphicsPipelines.all.empty());
    DUK_ASSERT(m_computePipelines.all.empty());
    DUK_ASSERT(m_commandQueues.all.empty());
}

void VulkanResourceManager::update(uint32_t imageIndex) {
    update(m_buffers, imageIndex);
    update(m_images, imageIndex);
    update(m_descriptorSets, imageIndex);
    update(m_frameBuffers, imageIndex);
    update(m_graphicsPipelines, imageIndex);
    update(m_computePipelines, imageIndex);
}

void VulkanResourceManager::clean(uint32_t imageIndex) {
    clean(m_frameBuffers, imageIndex);
    clean(m_descriptorSets, imageIndex);
    clean(m_images, imageIndex);
    clean(m_buffers, imageIndex);
    clean(m_graphicsPipelines, imageIndex);
    clean(m_computePipelines, imageIndex);
    clean(m_renderPasses, imageIndex);
    clean(m_commandQueues, imageIndex);
}

std::shared_ptr<VulkanBuffer> VulkanResourceManager::create(const VulkanBufferCreateInfo& bufferCreateInfo) {
    return create(m_buffers, bufferCreateInfo);
}

std::shared_ptr<VulkanMemoryImage> VulkanResourceManager::create(const VulkanMemoryImageCreateInfo& imageCreateInfo) {
    return create(m_images, imageCreateInfo);
}

std::shared_ptr<VulkanDescriptorSet> VulkanResourceManager::create(const VulkanDescriptorSetCreateInfo& descriptorSetCreateInfo) {
    return create(m_descriptorSets, descriptorSetCreateInfo);
}

std::shared_ptr<VulkanGraphicsPipeline> VulkanResourceManager::create(const VulkanGraphicsPipelineCreateInfo& pipelineCreateInfo) {
    return create(m_graphicsPipelines, pipelineCreateInfo);
}

std::shared_ptr<VulkanComputePipeline> VulkanResourceManager::create(const VulkanComputePipelineCreateInfo& pipelineCreateInfo) {
    return create(m_computePipelines, pipelineCreateInfo);
}

std::shared_ptr<VulkanFrameBuffer> VulkanResourceManager::create(const VulkanFrameBufferCreateInfo& frameBufferCreateInfo) {
    return create(m_frameBuffers, frameBufferCreateInfo);
}

std::shared_ptr<VulkanRenderPass> VulkanResourceManager::create(const VulkanRenderPassCreateInfo& renderPassCreateInfo) {
    return create(m_renderPasses, renderPassCreateInfo);
}

std::shared_ptr<VulkanCommandQueue> VulkanResourceManager::create(const VulkanCommandQueueCreateInfo& commandQueueCreateInfo) {
    return create(m_commandQueues, commandQueueCreateInfo);
}

std::set<uint32_t> VulkanResourceManager::image_indices_set() const {
    std::set<uint32_t> pendingIndices;
    for (auto i = 0; i < m_imageCount; i++) {
        pendingIndices.insert(i);
    }
    return pendingIndices;
}

void VulkanResourceManager::schedule_for_update(VulkanBuffer* buffer) {
    schedule_for_update(m_buffers, buffer);
}

void VulkanResourceManager::schedule_for_update(VulkanMemoryImage* image) {
    schedule_for_update(m_images, image);
}

void VulkanResourceManager::schedule_for_update(VulkanDescriptorSet* descriptorSet) {
    schedule_for_update(m_descriptorSets, descriptorSet);
}

void VulkanResourceManager::schedule_for_update(VulkanFrameBuffer* frameBuffer) {
    schedule_for_update(m_frameBuffers, frameBuffer);
}

void VulkanResourceManager::schedule_for_update(VulkanGraphicsPipeline* graphicsPipeline) {
    schedule_for_update(m_graphicsPipelines, graphicsPipeline);
}

}// namespace duk::rhi