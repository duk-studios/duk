/// vulkan_descriptor_set_allocator.h

#ifndef DUK_RHI_VULKAN_DESCRIPTOR_SET_ALLOCATOR_H
#define DUK_RHI_VULKAN_DESCRIPTOR_SET_ALLOCATOR_H

#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <unordered_map>

namespace duk::rhi {

struct VulkanDescriptorSetCacheCreateInfo {
    VkDevice device;
    uint32_t maxSets;
};

/// Caches VkDescriptorSet objects keyed by a caller-supplied hash.
/// Backed by a single persistent pool — sets are never individually freed.
/// A cache miss allocates a fresh set; a hit returns the existing one.
class VulkanDescriptorSetCache {
public:
    explicit VulkanDescriptorSetCache(const VulkanDescriptorSetCacheCreateInfo& createInfo);

    ~VulkanDescriptorSetCache();

    struct Result {
        VkDescriptorSet set;
        bool isNew;
    };

    DUK_NO_DISCARD Result get_or_allocate(VkDescriptorSetLayout layout, duk::hash::Hash hash);

private:
    VkDevice m_device;
    VkDescriptorPool m_pool{VK_NULL_HANDLE};
    std::unordered_map<duk::hash::Hash, VkDescriptorSet> m_cache;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_DESCRIPTOR_SET_ALLOCATOR_H

