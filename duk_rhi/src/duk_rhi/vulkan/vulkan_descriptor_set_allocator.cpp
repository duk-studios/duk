/// vulkan_descriptor_set_allocator.cpp

#include <duk_rhi/vulkan/vulkan_descriptor_set_allocator.h>

#include <stdexcept>

namespace duk::rhi {


VulkanDescriptorSetCache::VulkanDescriptorSetCache(const VulkanDescriptorSetCacheCreateInfo& createInfo)
    : m_device(createInfo.device) {

    const VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, createInfo.maxSets},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, createInfo.maxSets},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, createInfo.maxSets},
        {VK_DESCRIPTOR_TYPE_SAMPLER,                createInfo.maxSets},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          createInfo.maxSets},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          createInfo.maxSets},
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = createInfo.maxSets;
    poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
    poolInfo.pPoolSizes = poolSizes;

    if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_pool) != VK_SUCCESS) {
        throw std::runtime_error("VulkanDescriptorSetCache: failed to create VkDescriptorPool");
    }
}

VulkanDescriptorSetCache::~VulkanDescriptorSetCache() {
    if (m_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_pool, nullptr);
    }
}

VulkanDescriptorSetCache::Result VulkanDescriptorSetCache::get_or_allocate(
    VkDescriptorSetLayout layout, duk::hash::Hash hash) {

    auto it = m_cache.find(hash);
    if (it != m_cache.end()) {
        return {it->second, false};
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    if (vkAllocateDescriptorSets(m_device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("VulkanDescriptorSetCache: failed to allocate VkDescriptorSet");
    }

    m_cache.emplace(hash, descriptorSet);
    return {descriptorSet, true};
}

}// namespace duk::rhi

