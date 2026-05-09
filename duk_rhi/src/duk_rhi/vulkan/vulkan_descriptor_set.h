/// 01/06/2023
/// vulkan_descriptor_set.h

#ifndef DUK_RHI_VULKAN_DESCRIPTOR_SET_H
#define DUK_RHI_VULKAN_DESCRIPTOR_SET_H

#include <duk_rhi/descriptor_set.h>
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

private:
    static duk::hash::Hash calculate_hash(const DescriptorSetDescription& descriptorSetDescription);

    const CacheEntry& create_descriptor_set_layout(const DescriptorSetDescription& descriptorSetDescription, duk::hash::Hash hash);

private:
    VkDevice m_device;
    std::unordered_map<duk::hash::Hash, CacheEntry> m_descriptorLayoutCache;
};

struct VulkanDescriptorSetCreateInfo {
    VkDevice device;
    VulkanDescriptorSetLayoutCache* descriptorSetLayoutCache;
    VulkanSamplerCache* samplerCache;
    DescriptorSetDescription descriptorSetDescription;
};

class VulkanDescriptorSet : public DescriptorSet {
public:
    explicit VulkanDescriptorSet(const VulkanDescriptorSetCreateInfo& descriptorSetCreateInfo);

    ~VulkanDescriptorSet() override;

    /// Returns the single VkDescriptorSet handle.
    DUK_NO_DISCARD VkDescriptorSet handle();

    void set(uint32_t binding, const Descriptor& descriptor) override;

    DUK_NO_DISCARD uint32_t size() const noexcept override;

    DUK_NO_DISCARD Descriptor& at(uint32_t binding) override;

    DUK_NO_DISCARD const Descriptor& at(uint32_t binding) const override;

    DUK_NO_DISCARD Image* image(uint32_t binding) override;

    DUK_NO_DISCARD const Image* image(uint32_t binding) const override;

    DUK_NO_DISCARD Buffer* buffer(uint32_t binding) override;

    DUK_NO_DISCARD const Buffer* buffer(uint32_t binding) const override;

    void flush() override;

private:
    void clean();

private:
    VkDevice m_device;
    VulkanSamplerCache* m_samplerCache;
    DescriptorSetDescription m_descriptorSetDescription;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::vector<VkDescriptorSetLayoutBinding> m_descriptorBindings;
    std::vector<Descriptor> m_descriptors;
    VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
    VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_DESCRIPTOR_SET_H
