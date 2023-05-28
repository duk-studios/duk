/// 28/05/2023
/// vulkan_resource_manager.h

#ifndef DUK_RENDERER_VULKAN_RESOURCE_MANAGER_H
#define DUK_RENDERER_VULKAN_RESOURCE_MANAGER_H

#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_events.h>

#include <vector>
#include <memory>

namespace duk::renderer {

class VulkanMemoryImage;
class VulkanMemoryImageCreateInfo;
class VulkanFrameBuffer;
class VulkanFrameBufferCreateInfo;

struct VulkanResourceManagerCreateInfo {
    VulkanSwapchainCreateEvent* swapchainCreateEvent;
    VulkanSwapchainCleanEvent* swapchainCleanEvent;
};

class VulkanResourceManager {
public:

    explicit VulkanResourceManager(const VulkanResourceManagerCreateInfo& resourceManagerCreateInfo);

    ~VulkanResourceManager();

    std::shared_ptr<VulkanMemoryImage> create(const VulkanMemoryImageCreateInfo& imageCreateInfo);

    std::shared_ptr<VulkanFrameBuffer> create(const VulkanFrameBufferCreateInfo& frameBufferCreateInfo);

private:
    events::EventListener m_listener;
    std::vector<VulkanMemoryImage*> m_images;
    std::vector<VulkanFrameBuffer*> m_frameBuffers;
};

}

#endif // DUK_RENDERER_VULKAN_RESOURCE_MANAGER_H

