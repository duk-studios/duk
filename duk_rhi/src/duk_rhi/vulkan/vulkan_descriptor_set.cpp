/// 01/06/2023
/// vulkan_descriptor_set.cpp

#include <duk_rhi/vulkan/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_image.h>

#include <duk_hash/hash_combine.h>

#include <string>
#include <stdexcept>

namespace duk::rhi {

VkDescriptorType convert_descriptor_type(const BindingDescription& binding) {
    VkDescriptorType result;
    switch (binding.type) {
        case BindingType::UNIFORM_BUFFER:
            result = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            break;
        case BindingType::STORAGE_BUFFER:
            result = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            break;
        case BindingType::IMAGE:
            result = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            break;
        case BindingType::SAMPLER_IMAGE:
            result = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        case BindingType::STORAGE_IMAGE:
            result = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            break;
        default:
            throw std::runtime_error("Unsupported binding type in convert_descriptor_type: " + std::to_string(static_cast<int>(binding.type)));
    }
    return result;
}

VulkanDescriptorSetLayoutCache::VulkanDescriptorSetLayoutCache(const VulkanDescriptorSetLayoutCacheCreateInfo& descriptorSetLayoutCacheCreateInfo)
    : m_device(descriptorSetLayoutCacheCreateInfo.device) {
}

VulkanDescriptorSetLayoutCache::~VulkanDescriptorSetLayoutCache() {
    for (auto& [hash, entry]: m_descriptorLayoutCache) {
        vkDestroyPipelineLayout(m_device, entry.pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_device, entry.descriptorSetLayout, nullptr);
    }
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutCache::get_layout(const BindingLayout& bindingLayout) {
    auto hash = calculate_hash(bindingLayout);
    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.descriptorSetLayout;
    }
    return create_descriptor_set_layout(bindingLayout, hash).descriptorSetLayout;
}

const std::vector<VkDescriptorSetLayoutBinding>& VulkanDescriptorSetLayoutCache::get_bindings(const BindingLayout& bindingLayout) {
    auto hash = calculate_hash(bindingLayout);
    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.bindings;
    }
    return create_descriptor_set_layout(bindingLayout, hash).bindings;
}

VkPipelineLayout VulkanDescriptorSetLayoutCache::get_pipeline_layout(const BindingLayout& bindingLayout) {
    auto hash = calculate_hash(bindingLayout);
    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.pipelineLayout;
    }
    return create_descriptor_set_layout(bindingLayout, hash).pipelineLayout;
}

duk::hash::Hash VulkanDescriptorSetLayoutCache::calculate_hash(const BindingLayout& bindingLayout) {
    duk::hash::Hash hash = 0;
    for (auto& desc: bindingLayout) {
        duk::hash::hash_combine(hash, desc.type);
        duk::hash::hash_combine(hash, desc.moduleMask);
        duk::hash::hash_combine(hash, desc.name);
    }
    return hash;
}

const VulkanDescriptorSetLayoutCache::CacheEntry& VulkanDescriptorSetLayoutCache::create_descriptor_set_layout(const BindingLayout& bindingLayout, duk::hash::Hash hash) {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(bindingLayout.size());
    for (auto& desc: bindingLayout) {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = static_cast<uint32_t>(bindings.size());
        binding.descriptorCount = 1;
        binding.stageFlags = convert_module_mask(desc.moduleMask);
        binding.descriptorType = convert_descriptor_type(desc);
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    VkDescriptorSetLayout descriptorSetLayout;
    auto result = vkCreateDescriptorSetLayout(m_device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkDescriptorSetLayout");
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    VkPipelineLayout pipelineLayout;
    if (vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        vkDestroyDescriptorSetLayout(m_device, descriptorSetLayout, nullptr);
        throw std::runtime_error("failed to create VkPipelineLayout");
    }

    CacheEntry cacheEntry = {};
    cacheEntry.descriptorSetLayout = descriptorSetLayout;
    cacheEntry.pipelineLayout = pipelineLayout;
    cacheEntry.bindings = std::move(bindings);

    auto [it, inserted] = m_descriptorLayoutCache.emplace(hash, std::move(cacheEntry));
    if (!inserted) {
        throw std::runtime_error("failed to cache VkDescriptorSetLayout");
    }

    return it->second;
}

}// namespace duk::rhi