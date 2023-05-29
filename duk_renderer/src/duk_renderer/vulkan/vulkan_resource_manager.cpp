/// 28/05/2023
/// vulkan_resource_manager.cpp

#include <duk_renderer/vulkan/vulkan_resource_manager.h>
#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_image.h>
#include <duk_renderer/vulkan/vulkan_buffer.h>
#include <duk_renderer/vulkan/vulkan_swapchain.h>

namespace duk::renderer {

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
            detail::create(m_frameBuffers, m_imageCount);
        });

        m_listener.listen(*m_swapchain->clean_event(), [this] {
            detail::clean(m_frameBuffers);
            detail::clean(m_images);
            detail::clean(m_buffers);
            clean(m_frameBuffersToDelete);
            clean(m_imagesToDelete);
            clean(m_buffersToDelete);
        });
    }

    m_listener.listen(*resourceManagerCreateInfo.prepareFrameEvent, [this](uint32_t frameIndex) {
        clean(m_swapchain ? *m_swapchain->current_image_ptr() : frameIndex);
    });
}

VulkanResourceManager::~VulkanResourceManager() {
    assert(m_frameBuffers.empty());
    assert(m_images.empty());
    assert(m_buffers.empty());
}

void VulkanResourceManager::clean(uint32_t imageIndex) {
    clean(m_buffersToDelete, imageIndex);
    clean(m_imagesToDelete, imageIndex);
    clean(m_frameBuffersToDelete, imageIndex);
}

std::shared_ptr<VulkanBuffer> VulkanResourceManager::create(const VulkanBufferCreateInfo& bufferCreateInfo) {
    return create(m_buffers, m_buffersToDelete, bufferCreateInfo);
}

std::shared_ptr<VulkanMemoryImage> VulkanResourceManager::create(const VulkanMemoryImageCreateInfo& imageCreateInfo) {
    return create(m_images, m_imagesToDelete, imageCreateInfo);
}

std::shared_ptr<VulkanFrameBuffer> VulkanResourceManager::create(const VulkanFrameBufferCreateInfo& frameBufferCreateInfo) {
    return create(m_frameBuffers, m_frameBuffersToDelete, frameBufferCreateInfo);
}

std::set<uint32_t> VulkanResourceManager::image_indices_set() const {
    std::set<uint32_t> pendingIndices;
    for (auto i = 0; i < m_imageCount; i++) {
        pendingIndices.insert(i);
    }
    return pendingIndices;
}

}