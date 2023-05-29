/// 28/05/2023
/// vulkan_resource_manager.h

#ifndef DUK_RENDERER_VULKAN_RESOURCE_MANAGER_H
#define DUK_RENDERER_VULKAN_RESOURCE_MANAGER_H

#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_events.h>

#include <duk_macros/macros.h>

#include <vector>
#include <set>
#include <memory>

namespace duk::renderer {

class VulkanSwapchain;
class VulkanMemoryImage;
class VulkanMemoryImageCreateInfo;
class VulkanFrameBuffer;
class VulkanFrameBufferCreateInfo;
class VulkanBuffer;
class VulkanBufferCreateInfo;

struct VulkanResourceManagerCreateInfo {
    VulkanSwapchain* swapchain;
    uint32_t imageCount;
    VulkanPrepareFrameEvent* prepareFrameEvent;
};

class VulkanResourceManager {
public:

    explicit VulkanResourceManager(const VulkanResourceManagerCreateInfo& resourceManagerCreateInfo);

    ~VulkanResourceManager();

    void clean(uint32_t imageIndex);

    std::shared_ptr<VulkanBuffer> create(const VulkanBufferCreateInfo& bufferCreateInfo);

    std::shared_ptr<VulkanMemoryImage> create(const VulkanMemoryImageCreateInfo& imageCreateInfo);

    std::shared_ptr<VulkanFrameBuffer> create(const VulkanFrameBufferCreateInfo& frameBufferCreateInfo);
private:

    template<typename T>
    struct DeletionEntry {
        T* resource;
        std::set<uint32_t> pendingIndices;
    };

private:

    DUK_NO_DISCARD std::set<uint32_t> image_indices_set() const;

    template<typename ResourceT, typename CreateInfoT>
    std::shared_ptr<ResourceT> create(std::vector<ResourceT*>& resources, std::vector<DeletionEntry<ResourceT>>& resourcesToDelete, const CreateInfoT& createInfo) {
        auto deleter = [&resources, &resourcesToDelete, this](auto ptr) {
            resources.erase(std::remove(resources.begin(), resources.end(), ptr), resources.end());
            resourcesToDelete.push_back({ptr, image_indices_set()});
        };

        auto ptr = resources.emplace_back(new ResourceT(createInfo));

        return {ptr, deleter};
    }


    template<typename T>
    void clean(std::vector<DeletionEntry<T>>& resourcesToDelete, uint32_t imageIndex) {
        std::vector<DeletionEntry<T>> aliveResources;
        for (auto& deletionEntry : resourcesToDelete) {
            deletionEntry.resource->clean(imageIndex);
            deletionEntry.pendingIndices.erase(imageIndex);
            if (deletionEntry.pendingIndices.empty()) {
                delete deletionEntry.resource;
                deletionEntry.resource = nullptr;
            }
            else {
                aliveResources.push_back(deletionEntry);
            }
        }
        aliveResources.swap(resourcesToDelete);
    }

    template<typename T>
    void clean(std::vector<DeletionEntry<T>>& resourcesToDelete) {
        for (auto& deletionEntry : resourcesToDelete) {
            delete deletionEntry.resource;
        }
        resourcesToDelete.clear();
    }

private:
    VulkanSwapchain* m_swapchain;
    uint32_t m_imageCount;
    events::EventListener m_listener;
    std::vector<VulkanBuffer*> m_buffers;
    std::vector<DeletionEntry<VulkanBuffer>> m_buffersToDelete;
    std::vector<VulkanMemoryImage*> m_images;
    std::vector<DeletionEntry<VulkanMemoryImage>> m_imagesToDelete;
    std::vector<VulkanFrameBuffer*> m_frameBuffers;
    std::vector<DeletionEntry<VulkanFrameBuffer>> m_frameBuffersToDelete;
};

}

#endif // DUK_RENDERER_VULKAN_RESOURCE_MANAGER_H

