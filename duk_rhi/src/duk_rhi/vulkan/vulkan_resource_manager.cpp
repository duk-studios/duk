/// 28/05/2023
/// vulkan_resource_manager.cpp

#include <duk_rhi/vulkan/vulkan_resource_manager.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>

namespace duk::rhi {

namespace detail {

template<typename T>
void create(std::vector<T*>& resources, uint32_t imageCount) {
    for (auto& resource : resources) {
        resource->create(imageCount);
    }
}

template<typename T>
void clean(std::vector<T*>& resources) {
    for (auto& resource : resources) {
        resource->clean();
    }
}

}

VulkanResourceManager::VulkanResourceManager(const VulkanResourceManagerCreateInfo& resourceManagerCreateInfo) :
    m_swapchain(resourceManagerCreateInfo.swapchain),
    m_imageCount(resourceManagerCreateInfo.imageCount) {

    if (m_swapchain) {
        m_listener.listen(*m_swapchain->create_event(), [this](const auto& swapchainInfo) {
            m_imageCount = swapchainInfo.imageCount;
            detail::create(m_buffers, m_imageCount);
            detail::create(m_images, m_imageCount);
            detail::create(m_descriptorSets, m_imageCount);
            detail::create(m_frameBuffers, m_imageCount);
            detail::create(m_graphicsPipelines, m_imageCount);
        });

        m_listener.listen(*m_swapchain->clean_event(), [this] {
            detail::clean(m_frameBuffers);
            detail::clean(m_descriptorSets);
            detail::clean(m_images);
            detail::clean(m_buffers);
            detail::clean(m_graphicsPipelines);
            clean(m_frameBuffersToDelete);
            clean(m_descriptorSetsToDelete);
            clean(m_graphicsPipelinesToDelete);
            clean(m_computePipelinesToDelete);
            clean(m_imagesToDelete);
            clean(m_buffersToDelete);
            clean(m_renderPassesToDelete);
        });
    }

    m_listener.listen(*resourceManagerCreateInfo.prepareFrameEvent, [this](uint32_t frameIndex) {
        clean(m_swapchain ? *m_swapchain->current_image_ptr() : frameIndex);
    });
}

VulkanResourceManager::~VulkanResourceManager() {
    assert(m_frameBuffers.empty());
    assert(m_descriptorSets.empty());
    assert(m_images.empty());
    assert(m_buffers.empty());
    assert(m_graphicsPipelines.empty());
    assert(m_computePipelines.empty());
}

void VulkanResourceManager::clean(uint32_t imageIndex) {
    clean(m_descriptorSetsToDelete, imageIndex);
    clean(m_frameBuffersToDelete, imageIndex);
    clean(m_imagesToDelete, imageIndex);
    clean(m_buffersToDelete, imageIndex);
    clean(m_graphicsPipelinesToDelete, imageIndex);
    clean(m_computePipelinesToDelete, imageIndex);
    clean(m_renderPassesToDelete, imageIndex);
}

std::shared_ptr<VulkanBuffer> VulkanResourceManager::create(const VulkanBufferCreateInfo& bufferCreateInfo) {
    return create(m_buffers, m_buffersToDelete, bufferCreateInfo);
}

std::shared_ptr<VulkanMemoryImage> VulkanResourceManager::create(const VulkanMemoryImageCreateInfo& imageCreateInfo) {
    return create(m_images, m_imagesToDelete, imageCreateInfo);
}

std::shared_ptr<VulkanDescriptorSet> VulkanResourceManager::create(const VulkanDescriptorSetCreateInfo& descriptorSetCreateInfo) {
    return create(m_descriptorSets, m_descriptorSetsToDelete, descriptorSetCreateInfo);
}

std::shared_ptr<VulkanGraphicsPipeline> VulkanResourceManager::create(const VulkanGraphicsPipelineCreateInfo& pipelineCreateInfo) {
    return create(m_graphicsPipelines, m_graphicsPipelinesToDelete, pipelineCreateInfo);
}

std::shared_ptr<VulkanComputePipeline> VulkanResourceManager::create(const VulkanComputePipelineCreateInfo& pipelineCreateInfo) {
    return create(m_computePipelines, m_computePipelinesToDelete, pipelineCreateInfo);
}

std::shared_ptr<VulkanFrameBuffer> VulkanResourceManager::create(const VulkanFrameBufferCreateInfo& frameBufferCreateInfo) {
    return create(m_frameBuffers, m_frameBuffersToDelete, frameBufferCreateInfo);
}

std::shared_ptr<VulkanRenderPass> VulkanResourceManager::create(const VulkanRenderPassCreateInfo& renderPassCreateInfo) {
    return create(m_renderPasses, m_renderPassesToDelete, renderPassCreateInfo);
}

std::set<uint32_t> VulkanResourceManager::image_indices_set() const {
    std::set<uint32_t> pendingIndices;
    for (auto i = 0; i < m_imageCount; i++) {
        pendingIndices.insert(i);
    }
    return pendingIndices;
}
}