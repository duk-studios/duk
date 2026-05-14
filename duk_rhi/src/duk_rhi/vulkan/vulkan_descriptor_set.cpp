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
    return std::visit([](auto&& b) -> VkDescriptorType {
        using T = std::decay_t<decltype(b)>;
        if constexpr (std::is_same_v<T, BufferBindingDescription>) {
            switch (b.type) {
                case BufferBindingType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                case BufferBindingType::STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                default: throw std::logic_error("unsupported BufferBindingType for VkDescriptorType conversion");
            }
        } else {
            static_assert(std::is_same_v<T, ImageBindingDescription>);
            switch (b.type) {
                case ImageBindingType::IMAGE:         return VK_DESCRIPTOR_TYPE_SAMPLER;
                case ImageBindingType::IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                case ImageBindingType::STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                default: throw std::logic_error("unsupported ImageBindingType for VkDescriptorType conversion");
            }
        }
    }, binding.binding);
}

VulkanDescriptorSetLayoutCache::VulkanDescriptorSetLayoutCache(
    const VulkanDescriptorSetLayoutCacheCreateInfo& descriptorSetLayoutCacheCreateInfo)
    : m_device(descriptorSetLayoutCacheCreateInfo.device) {
}

VulkanDescriptorSetLayoutCache::~VulkanDescriptorSetLayoutCache() {
    for (auto& [hash, entry] : m_descriptorLayoutCache) {
        vkDestroyPipelineLayout(m_device, entry.pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_device, entry.descriptorSetLayout, nullptr);
    }
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutCache::get_layout(const ShaderBindingLayout& bindingLayout) {
    auto hash = calculate_hash(bindingLayout);
    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.descriptorSetLayout;
    }
    return create_descriptor_set_layout(bindingLayout, hash).descriptorSetLayout;
}

const std::vector<VkDescriptorSetLayoutBinding>& VulkanDescriptorSetLayoutCache::get_bindings(
    const ShaderBindingLayout& bindingLayout) {
    auto hash = calculate_hash(bindingLayout);
    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.bindings;
    }
    return create_descriptor_set_layout(bindingLayout, hash).bindings;
}

VkPipelineLayout VulkanDescriptorSetLayoutCache::get_pipeline_layout(const ShaderBindingLayout& bindingLayout) {
    auto hash = calculate_hash(bindingLayout);
    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.pipelineLayout;
    }
    return create_descriptor_set_layout(bindingLayout, hash).pipelineLayout;
}

duk::hash::Hash VulkanDescriptorSetLayoutCache::calculate_hash(const ShaderBindingLayout& bindingLayout) {
    duk::hash::Hash hash = 0;
    for (auto& desc : bindingLayout) {
        duk::hash::hash_combine(hash, desc.binding.index());
        duk::hash::hash_combine(hash, desc.moduleMask);
        duk::hash::hash_combine(hash, desc.name);
        std::visit([&hash](auto&& b) {
            duk::hash::hash_combine(hash, static_cast<uint32_t>(b.type));
        }, desc.binding);
    }
    return hash;
}

const VulkanDescriptorSetLayoutCache::CacheEntry& VulkanDescriptorSetLayoutCache::create_descriptor_set_layout(
    const ShaderBindingLayout& bindingLayout, duk::hash::Hash hash) {

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(bindingLayout.size());
    for (auto& desc : bindingLayout) {
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