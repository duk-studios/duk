/// 01/06/2023
/// vulkan_descriptor_set.h

#ifndef DUK_RENDERER_VULKAN_DESCRIPTOR_SET_H
#define DUK_RENDERER_VULKAN_DESCRIPTOR_SET_H

#include <duk_renderer/descriptor_set.h>
#include <duk_renderer/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>
#include <duk_tools/singleton.h>
#include <duk_hash/hash_combine.h>

namespace duk::renderer {

namespace vk {

VkDescriptorType convert_descriptor_type(DescriptorType descriptorType);

}

struct VulkanDescriptorSetLayoutCacheCreateInfo {
    VkDevice device;
};

class VulkanDescriptorSetLayoutCache {
public:

    explicit VulkanDescriptorSetLayoutCache(const VulkanDescriptorSetLayoutCacheCreateInfo& descriptorSetLayoutCacheCreateInfo);

    ~VulkanDescriptorSetLayoutCache();

    DUK_NO_DISCARD VkDescriptorSetLayout get(const DescriptorSetDescription& descriptorSetDescription);

private:

    static duk::hash::Hash calculate_hash(const DescriptorSetDescription& descriptorSetDescription);

    VkDescriptorSetLayout create_descriptor_set_layout(const DescriptorSetDescription& descriptorSetDescription, duk::hash::Hash hash);

private:

    struct CacheEntry {
        VkDescriptorSetLayout descriptorSetLayout;
    };

    VkDevice m_device;
    std::unordered_map<duk::hash::Hash, CacheEntry> m_descriptorLayoutCache;

};

}

#endif // DUK_RENDERER_VULKAN_DESCRIPTOR_SET_H

