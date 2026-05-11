/// 01/06/2023
/// vulkan_descriptor_set.h

#ifndef DUK_RHI_VULKAN_DESCRIPTOR_SET_H
#define DUK_RHI_VULKAN_DESCRIPTOR_SET_H

#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_sampler.h>

#include <duk_hash/hash_combine.h>
#include <duk_macros/macros.h>

namespace duk::rhi {

VkDescriptorType convert_descriptor_type(DescriptorType descriptorType);

struct VulkanDescriptorSetLayoutCacheCreateInfo {
    VkDevice device;
};

class VulkanDescriptorSetLayoutCache {
public:
    explicit VulkanDescriptorSetLayoutCache(const VulkanDescriptorSetLayoutCacheCreateInfo& descriptorSetLayoutCacheCreateInfo);

    ~VulkanDescriptorSetLayoutCache();

    DUK_NO_DISCARD VkDescriptorSetLayout get_layout(const DescriptorSetDescription& descriptorSetDescription);

    DUK_NO_DISCARD const std::vector<VkDescriptorSetLayoutBinding>& get_bindings(const DescriptorSetDescription& descriptorSetDescription);

private:
    struct CacheEntry {
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
    };

    static duk::hash::Hash calculate_hash(const DescriptorSetDescription& descriptorSetDescription);

    const CacheEntry& create_descriptor_set_layout(const DescriptorSetDescription& descriptorSetDescription, duk::hash::Hash hash);

    VkDevice m_device;
    std::unordered_map<duk::hash::Hash, CacheEntry> m_descriptorLayoutCache;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_DESCRIPTOR_SET_H
