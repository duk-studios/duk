/// 14/05/2023
/// vulkan_resource_pool.h

#ifndef DUK_RENDERER_VULKAN_RESOURCE_POOL_H
#define DUK_RENDERER_VULKAN_RESOURCE_POOL_H

#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_macros/assert.h>

#include <vector>

namespace duk::renderer {

template<typename T>
class VulkanHandlePool {
public:

    VulkanHandlePool() :
        m_allocationCount(0) {

    }

    virtual ~VulkanHandlePool() {
        DUK_ASSERT(m_allocationCount == 0, "memory leak on VulkanHandlePool destructor");
    }

    T allocate() {
        m_allocationCount++;
        if (m_resources.empty()) {
            expand(m_resources.size() + 1);
        }

        auto resource = m_resources.back();
        m_resources.pop_back();
        return resource;
    }

    void free(T& resource) {
        DUK_ASSERT(m_allocationCount > 0, "tried to free a resource on a VulkanHandlePool with no allocations");
        m_allocationCount--;
        m_resources.push_back(resource);
        resource = VK_NULL_HANDLE;
    }

    void free(T* resources, size_t resourceCount) {
        for (auto i = 0; i < resourceCount; i++) {
            free(resources[i]);
        }
    }

    void free(std::vector<T>& resources) {
        free(resources.data(), resources.size());
    }

    virtual void expand(size_t n) = 0;

protected:
    std::vector<T> m_resources;
    size_t m_allocationCount;
};

struct VulkanFencePoolCreateInfo {
    VkDevice device;
    VkFenceCreateFlags flags;
};

class VulkanFencePool : public VulkanHandlePool<VkFence> {
public:
    explicit VulkanFencePool(const VulkanFencePoolCreateInfo& fencePoolCreateInfo);

    ~VulkanFencePool();

    void expand(size_t n) override;

    void clean();
private:
    VkDevice m_device;
    VkFenceCreateFlags m_flags;
};

struct VulkanSemaphorePoolCreateInfo {
    VkDevice device;
};

class VulkanSemaphorePool : public VulkanHandlePool<VkSemaphore> {
public:
    explicit VulkanSemaphorePool(const VulkanSemaphorePoolCreateInfo& semaphorePoolCreateInfo);

    ~VulkanSemaphorePool();

    void expand(size_t n) override;

    void clean();
private:
    VkDevice m_device;
};


struct VulkanCommandBufferPoolCreateInfo {
    VkDevice device;
    VkCommandPool commandPool;
};

class VulkanCommandBufferPool : public VulkanHandlePool<VkCommandBuffer> {
public:
    explicit VulkanCommandBufferPool(const VulkanCommandBufferPoolCreateInfo& commandBufferPoolCreateInfo);

    ~VulkanCommandBufferPool();

    void expand(size_t n) override;

    void clean();
private:
    VkDevice m_device;
    VkCommandPool m_commandPool;
};


}

#endif // DUK_RENDERER_VULKAN_RESOURCE_POOL_H

