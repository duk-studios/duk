/// 28/05/2023
/// vulkan_resource_manager.cpp

#include <duk_renderer/vulkan/vulkan_resource_manager.h>
#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_image.h>

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

template<typename ResourceT, typename CreateInfoT>
std::shared_ptr<ResourceT> create(std::vector<ResourceT*>& resources, const CreateInfoT& createInfo) {
    auto deleter = [&resources](auto ptr) {
        resources.erase(std::remove(resources.begin(), resources.end(), ptr), resources.end());
        delete ptr;
    };

    auto ptr = resources.emplace_back(new ResourceT(createInfo));

    return {ptr, deleter};
}
}

VulkanResourceManager::VulkanResourceManager(const VulkanResourceManagerCreateInfo& resourceManagerCreateInfo) {
    m_listener.listen(*resourceManagerCreateInfo.swapchainCreateEvent, [this](const auto& swapchainInfo) {
        detail::create(m_images, swapchainInfo.imageCount);
        detail::create(m_frameBuffers, swapchainInfo.imageCount);
    });

    m_listener.listen(*resourceManagerCreateInfo.swapchainCleanEvent, [this] {
        detail::clean(m_frameBuffers);
        detail::clean(m_images);
    });
}

VulkanResourceManager::~VulkanResourceManager() {
    assert(m_images.empty());
    assert(m_frameBuffers.empty());
}

std::shared_ptr<VulkanMemoryImage> VulkanResourceManager::create(const VulkanMemoryImageCreateInfo& imageCreateInfo) {
    return detail::create(m_images, imageCreateInfo);
}

std::shared_ptr<VulkanFrameBuffer> VulkanResourceManager::create(const VulkanFrameBufferCreateInfo& frameBufferCreateInfo) {
    return detail::create(m_frameBuffers, frameBufferCreateInfo);
}

}