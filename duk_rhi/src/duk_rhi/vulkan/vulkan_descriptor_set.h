/// 01/06/2023
/// vulkan_descriptor_set.h

#ifndef DUK_RHI_VULKAN_DESCRIPTOR_SET_H
#define DUK_RHI_VULKAN_DESCRIPTOR_SET_H

#include <duk_rhi/shader.h>
#include <duk_rhi/binding_layout.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_sampler.h>

#include <duk_hash/hash_combine.h>
#include <duk_macros/macros.h>

namespace duk::rhi {

VkDescriptorType convert_descriptor_type(const BindingDescription& binding);

struct VulkanDescriptorSetLayoutCacheCreateInfo {
    VkDevice device;
};

/// Caches VkDescriptorSetLayout objects keyed by the ShaderBindingLayout that describes them.
/// All descriptors are placed in a single VkDescriptorSetLayout (set=0).
class VulkanDescriptorSetLayoutCache {
public:
    explicit VulkanDescriptorSetLayoutCache(const VulkanDescriptorSetLayoutCacheCreateInfo& descriptorSetLayoutCacheCreateInfo);

    ~VulkanDescriptorSetLayoutCache();

    DUK_NO_DISCARD VkDescriptorSetLayout get_layout(const BindingLayout& bindingLayout);

    DUK_NO_DISCARD const std::vector<VkDescriptorSetLayoutBinding>& get_bindings(const BindingLayout& bindingLayout);

    DUK_NO_DISCARD VkPipelineLayout get_pipeline_layout(const BindingLayout& bindingLayout);

private:
    struct CacheEntry {
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
    };

    static duk::hash::Hash calculate_hash(const BindingLayout& bindingLayout);

    const CacheEntry& create_descriptor_set_layout(const BindingLayout& bindingLayout, duk::hash::Hash hash);

    VkDevice m_device;
    std::unordered_map<duk::hash::Hash, CacheEntry> m_descriptorLayoutCache;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_DESCRIPTOR_SET_H
