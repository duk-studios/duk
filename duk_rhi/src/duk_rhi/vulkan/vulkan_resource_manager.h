/// 28/05/2023
/// vulkan_resource_manager.h

#ifndef DUK_RHI_VULKAN_RESOURCE_MANAGER_H
#define DUK_RHI_VULKAN_RESOURCE_MANAGER_H

#include <duk_rhi/vulkan/vulkan_events.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <memory>
#include <set>
#include <vector>

namespace duk::rhi {

class VulkanSwapchain;
class VulkanMemoryImage;
class VulkanMemoryImageCreateInfo;
class VulkanBuffer;
class VulkanBufferCreateInfo;
class VulkanDescriptorSet;
class VulkanDescriptorSetCreateInfo;
class VulkanGraphicsPipeline;
class VulkanGraphicsPipelineCreateInfo;
class VulkanComputePipeline;
class VulkanComputePipelineCreateInfo;
class VulkanFrameBuffer;
class VulkanFrameBufferCreateInfo;
class VulkanRenderPass;
class VulkanRenderPassCreateInfo;

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

    void update(uint32_t imageIndex);

    std::shared_ptr<VulkanBuffer> create(const VulkanBufferCreateInfo& bufferCreateInfo);

    std::shared_ptr<VulkanMemoryImage> create(const VulkanMemoryImageCreateInfo& imageCreateInfo);

    std::shared_ptr<VulkanDescriptorSet> create(const VulkanDescriptorSetCreateInfo& descriptorSetCreateInfo);

    std::shared_ptr<VulkanGraphicsPipeline> create(const VulkanGraphicsPipelineCreateInfo& pipelineCreateInfo);

    std::shared_ptr<VulkanComputePipeline> create(const VulkanComputePipelineCreateInfo& pipelineCreateInfo);

    std::shared_ptr<VulkanFrameBuffer> create(const VulkanFrameBufferCreateInfo& frameBufferCreateInfo);

    std::shared_ptr<VulkanRenderPass> create(const VulkanRenderPassCreateInfo& renderPassCreateInfo);

    std::shared_ptr<VulkanCommandQueue> create(const VulkanCommandQueueCreateInfo& commandQueueCreateInfo);

    void schedule_for_update(VulkanBuffer* buffer);

    void schedule_for_update(VulkanMemoryImage* image);

    void schedule_for_update(VulkanDescriptorSet* descriptorSet);

    void schedule_for_update(VulkanFrameBuffer* frameBuffer);

    void schedule_for_update(VulkanGraphicsPipeline* graphicsPipeline);

private:
    template<typename T>
    struct ResourceSet {
        std::vector<T*> all;
        std::unordered_map<T*, std::set<uint32_t>> toDelete;
        std::unordered_map<T*, std::set<uint32_t>> toUpdate;
    };

private:
    DUK_NO_DISCARD std::set<uint32_t> image_indices_set() const;

    template<typename T, typename CreateInfoT>
    std::shared_ptr<T> create(ResourceSet<T>& resourceSet, const CreateInfoT& createInfo) {
        auto deleter = [&resourceSet, this](auto ptr) {
            std::erase(resourceSet.all, ptr);
            resourceSet.toUpdate.erase(ptr);
            resourceSet.toDelete.emplace(ptr, image_indices_set());
        };

        auto ptr = resourceSet.all.emplace_back(new T(createInfo));

        return {ptr, deleter};
    }

    template<typename T>
    void update(ResourceSet<T>& resourceSet, uint32_t imageIndex) {
        std::unordered_map<T*, std::set<uint32_t>> pendingResources;
        for (auto& entry: resourceSet.toUpdate) {
            auto& [resource, pendingImages] = entry;
            resource->update(imageIndex);
            pendingImages.erase(imageIndex);
            if (!pendingImages.empty()) {
                pendingResources.insert(entry);
            }
        }
        std::swap(pendingResources, resourceSet.toUpdate);
    }

    template<typename T>
    void clean(ResourceSet<T>& resourceSet, uint32_t imageIndex) {
        std::unordered_map<T*, std::set<uint32_t>> pendingResources;
        for (auto& entry: resourceSet.toDelete) {
            auto& [resource, pendingIndices] = entry;
            resource->clean(imageIndex);
            pendingIndices.erase(imageIndex);
            if (pendingIndices.empty()) {
                delete resource;
            } else {
                pendingResources.insert(entry);
            }
        }
        std::swap(pendingResources, resourceSet.toDelete);
    }

    template<typename T>
    void clean(ResourceSet<T>& resourceSet) {
        for (auto& entry: resourceSet.toDelete) {
            delete entry.first;
        }
        resourceSet.toDelete.clear();
    }

    template<typename T>
    void schedule_for_update(ResourceSet<T>& resourceSet, T* resource) {
        auto& imagesToUpdate = resourceSet.toUpdate[resource];
        imagesToUpdate = image_indices_set();
    }

private:
    VulkanSwapchain* m_swapchain;
    uint32_t m_imageCount;
    event::Listener m_listener;
    ResourceSet<VulkanBuffer> m_buffers;
    ResourceSet<VulkanMemoryImage> m_images;
    ResourceSet<VulkanDescriptorSet> m_descriptorSets;
    ResourceSet<VulkanFrameBuffer> m_frameBuffers;
    ResourceSet<VulkanGraphicsPipeline> m_graphicsPipelines;
    ResourceSet<VulkanComputePipeline> m_computePipelines;
    ResourceSet<VulkanRenderPass> m_renderPasses;
    ResourceSet<VulkanCommandQueue> m_commandQueues;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_RESOURCE_MANAGER_H
