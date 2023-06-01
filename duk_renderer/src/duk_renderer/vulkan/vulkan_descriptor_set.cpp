/// 01/06/2023
/// vulkan_descriptor_set.cpp

#include <duk_renderer/vulkan/vulkan_descriptor_set.h>
#include <duk_renderer/vulkan/pipeline/vulkan_shader.h>

#include <stdexcept>

namespace duk::renderer {

namespace vk {

VkDescriptorType convert_descriptor_type(DescriptorType descriptorType) {
    VkDescriptorType converted;
    switch(descriptorType) {
        case DescriptorType::UNIFORM_BUFFER: converted = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
        case DescriptorType::STORAGE_BUFFER: converted = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; break;
        case DescriptorType::IMAGE: converted = VK_DESCRIPTOR_TYPE_SAMPLER; break;
        case DescriptorType::IMAGE_SAMPLER: converted = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
        default:
            throw std::logic_error("tried to convert a unsupported DescriptorType");
    }
    return converted;
}

}

VulkanDescriptorSetLayoutCache::VulkanDescriptorSetLayoutCache(const VulkanDescriptorSetLayoutCacheCreateInfo& descriptorSetLayoutCacheCreateInfo) :
    m_device(descriptorSetLayoutCacheCreateInfo.device) {

}

VulkanDescriptorSetLayoutCache::~VulkanDescriptorSetLayoutCache() {
    for (auto& [hash, entry] : m_descriptorLayoutCache) {
        vkDestroyDescriptorSetLayout(m_device, entry.descriptorSetLayout, nullptr);
    }
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutCache::get(const DescriptorSetDescription& descriptorSetDescription) {

    auto hash = calculate_hash(descriptorSetDescription);

    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.descriptorSetLayout;
    }

    return create_descriptor_set_layout(descriptorSetDescription, hash);
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutCache::create_descriptor_set_layout(const DescriptorSetDescription& descriptorSetDescription, duk::hash::Hash hash) {

    const auto& descriptorBindings = descriptorSetDescription.bindings;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(descriptorBindings.size());
    for (auto& descriptorBinding: descriptorBindings) {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = bindings.size();
        binding.descriptorCount = 1;
        binding.stageFlags = vk::convert_module_mask(descriptorBinding.moduleMask);
        binding.descriptorType = vk::convert_descriptor_type(descriptorBinding.type);
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = descriptorSetDescription.bindings.size();
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    VkDescriptorSetLayout descriptorSetLayout;
    auto result = vkCreateDescriptorSetLayout(m_device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkDescriptorSetLayout");
    }

    m_descriptorLayoutCache.emplace(hash, CacheEntry{descriptorSetLayout});

    return descriptorSetLayout;
}

duk::hash::Hash VulkanDescriptorSetLayoutCache::calculate_hash(const DescriptorSetDescription& descriptorSetDescription) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, descriptorSetDescription);
    return hash;
}

}