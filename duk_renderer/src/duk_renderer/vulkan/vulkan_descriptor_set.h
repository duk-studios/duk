/// 01/06/2023
/// vulkan_descriptor_set.h

#ifndef DUK_RENDERER_VULKAN_DESCRIPTOR_SET_H
#define DUK_RENDERER_VULKAN_DESCRIPTOR_SET_H

#include <duk_renderer/descriptor_set.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_sampler.h>

#include <duk_macros/macros.h>
#include <duk_hash/hash_combine.h>

namespace duk::renderer {

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
    uint32_t imageCount;
    DescriptorSetDescription descriptorSetDescription;
};

class VulkanDescriptorSet : public DescriptorSet {
public:

    explicit VulkanDescriptorSet(const VulkanDescriptorSetCreateInfo& descriptorSetCreateInfo);

    ~VulkanDescriptorSet() override;

    void create(uint32_t imageCount);

    void clean();

    void clean(uint32_t imageIndex);

    void update(uint32_t imageIndex);

    DUK_NO_DISCARD VkDescriptorSet handle(uint32_t imageIndex);

    void set(uint32_t binding, const Descriptor& descriptor) override;

    DUK_NO_DISCARD Image* image(uint32_t binding) override;

    DUK_NO_DISCARD const Image* image(uint32_t binding) const override;

    DUK_NO_DISCARD Buffer* buffer(uint32_t binding) override;

    DUK_NO_DISCARD const Buffer* buffer(uint32_t binding) const override;

    void flush() override;

private:

    void update_hash();

    void update_descriptors_and_set(uint32_t imageIndex);

    void update_descriptors(uint32_t imageIndex);

private:
    VkDevice m_device;
    VulkanSamplerCache* m_samplerCache;
    DescriptorSetDescription m_descriptorSetDescription;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::vector<VkDescriptorSetLayoutBinding> m_descriptorBindings;
    std::vector<Descriptor> m_descriptors;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    duk::hash::Hash m_descriptorSetHash;
    std::vector<duk::hash::Hash> m_descriptorSetHashes;
    duk::events::EventListener m_listener;

};

}

#endif // DUK_RENDERER_VULKAN_DESCRIPTOR_SET_H

