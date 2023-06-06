/// 01/06/2023
/// vulkan_descriptor_set.cpp

#include <duk_renderer/vulkan/vulkan_descriptor_set.h>
#include <duk_renderer/vulkan/pipeline/vulkan_shader.h>
#include <duk_renderer/vulkan/vulkan_buffer.h>
#include <duk_renderer/vulkan/vulkan_image.h>

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

VkDescriptorSetLayout VulkanDescriptorSetLayoutCache::get_layout(const DescriptorSetDescription& descriptorSetDescription) {

    auto hash = calculate_hash(descriptorSetDescription);

    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.descriptorSetLayout;
    }

    return create_descriptor_set_layout(descriptorSetDescription, hash).descriptorSetLayout;
}


const std::vector<VkDescriptorSetLayoutBinding>& VulkanDescriptorSetLayoutCache::get_bindings(const DescriptorSetDescription& descriptorSetDescription) {
    auto hash = calculate_hash(descriptorSetDescription);

    auto it = m_descriptorLayoutCache.find(hash);
    if (it != m_descriptorLayoutCache.end()) {
        return it->second.bindings;
    }

    return create_descriptor_set_layout(descriptorSetDescription, hash).bindings;
}


const VulkanDescriptorSetLayoutCache::CacheEntry& VulkanDescriptorSetLayoutCache::create_descriptor_set_layout(const DescriptorSetDescription& descriptorSetDescription, duk::hash::Hash hash) {

    const auto& descriptorBindings = descriptorSetDescription.bindings;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(descriptorBindings.size());
    for (auto& descriptorBinding : descriptorBindings) {
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

    CacheEntry cacheEntry = {};
    cacheEntry.descriptorSetLayout = descriptorSetLayout;
    cacheEntry.bindings = std::move(bindings);

    auto[it, inserted] = m_descriptorLayoutCache.emplace(hash, std::move(cacheEntry));

    if (!inserted) {
        throw std::runtime_error("failed to cache VkDescriptorSetLayout");
    }

    return it->second;
}

duk::hash::Hash VulkanDescriptorSetLayoutCache::calculate_hash(const DescriptorSetDescription& descriptorSetDescription) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, descriptorSetDescription);
    return hash;
}

VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDescriptorSetCreateInfo& descriptorSetCreateInfo) :
    m_device(descriptorSetCreateInfo.device),
    m_samplerCache(descriptorSetCreateInfo.samplerCache),
    m_descriptorSetDescription(descriptorSetCreateInfo.descriptorSetDescription),
    m_descriptorSetLayout(descriptorSetCreateInfo.descriptorSetLayoutCache->get_layout(m_descriptorSetDescription)),
    m_descriptorBindings(descriptorSetCreateInfo.descriptorSetLayoutCache->get_bindings(m_descriptorSetDescription)),
    m_descriptors(m_descriptorBindings.size()),
    m_descriptorSetHash(duk::hash::UndefinedHash),
    m_descriptorSetHashes(descriptorSetCreateInfo.imageCount, m_descriptorSetHash) {
    create(descriptorSetCreateInfo.imageCount);
}

VulkanDescriptorSet::~VulkanDescriptorSet() {
    clean();
}

void VulkanDescriptorSet::create(uint32_t imageCount) {
    m_descriptorSets.resize(imageCount);

    std::vector<VkDescriptorPoolSize> poolSizes = {};
    poolSizes.resize(m_descriptorBindings.size());
    for (size_t i = 0; i < m_descriptorBindings.size(); i++) {
        poolSizes[i].type = m_descriptorBindings[i].descriptorType;
        poolSizes[i].descriptorCount = imageCount;
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
    descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(imageCount);
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    auto result = vkCreateDescriptorPool(m_device, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkDescriptorPool");
    }

    std::vector<VkDescriptorSetLayout> layouts(imageCount, m_descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = imageCount;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(imageCount);
    result = vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate VkDescriptorSet");
    }
}

void VulkanDescriptorSet::clean() {
    for (int i = 0; i < m_descriptorSets.size(); i++) {
        clean(i);
    }
    m_descriptorSets.clear();

    vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
}

void VulkanDescriptorSet::clean(uint32_t imageIndex) {
    auto& descriptorSet = m_descriptorSets[imageIndex];
    if (descriptorSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &descriptorSet);
        descriptorSet = VK_NULL_HANDLE;
    }
}

void VulkanDescriptorSet::update(uint32_t imageIndex) {
    assert(m_descriptorSetHash != duk::hash::UndefinedHash);

    if (m_descriptorSetHashes[imageIndex] == m_descriptorSetHash) {
        return;
    }

    m_descriptorSetHashes[imageIndex] = imageIndex;

    auto& descriptorSet = m_descriptorSets[imageIndex];
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;
    std::vector<VkWriteDescriptorSet> writeDescriptors;

    for (auto& descriptor : m_descriptors) {
        VkWriteDescriptorSet writeDescriptor = {};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.dstBinding = writeDescriptors.size();
        writeDescriptor.descriptorType = vk::convert_descriptor_type(descriptor.type());
        writeDescriptor.descriptorCount = 1;
        writeDescriptor.dstArrayElement = 0;
        writeDescriptor.dstSet = descriptorSet;

        switch (descriptor.type()) {
            case DescriptorType::UNIFORM_BUFFER:
            case DescriptorType::STORAGE_BUFFER: {
                auto buffer = dynamic_cast<VulkanBuffer*>(descriptor.buffer());

                buffer->update(imageIndex);

                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = buffer->handle(imageIndex);
                bufferInfo.offset = 0;
                bufferInfo.range = buffer->size();
                bufferInfos.push_back(bufferInfo);
                writeDescriptor.pBufferInfo = &bufferInfos.back();
                break;
            }
            case DescriptorType::IMAGE:
            case DescriptorType::IMAGE_SAMPLER: {
                auto image = dynamic_cast<VulkanImage*>(descriptor.image());
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageView = image->image_view(imageIndex);
                imageInfo.imageLayout = vk::convert_layout(descriptor.image_layout());
                if (descriptor.type() == DescriptorType::IMAGE_SAMPLER) {
                    imageInfo.sampler = m_samplerCache->get(descriptor.sampler());
                }
                imageInfos.push_back(imageInfo);
                writeDescriptor.pImageInfo = &imageInfos.back();
                break;
            }
            default:
                throw std::logic_error("un-handled DescriptorType");
        }
        writeDescriptors.push_back(writeDescriptor);
    }

    vkUpdateDescriptorSets(m_device, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
}

VkDescriptorSet VulkanDescriptorSet::handle(uint32_t imageIndex) {
    return m_descriptorSets[imageIndex];
}

Descriptor& VulkanDescriptorSet::at(uint32_t binding) {
    return m_descriptors.at(binding);
}

const Descriptor& VulkanDescriptorSet::at(uint32_t binding) const {
    return m_descriptors.at(binding);
}

Image* VulkanDescriptorSet::image(uint32_t binding) {
    return m_descriptors[binding].image();
}

const Image* VulkanDescriptorSet::image(uint32_t binding) const {
    return m_descriptors[binding].image();
}

Buffer* VulkanDescriptorSet::buffer(uint32_t binding) {
    return m_descriptors[binding].buffer();
}

const Buffer* VulkanDescriptorSet::buffer(uint32_t binding) const {
    return m_descriptors[binding].buffer();
}

void VulkanDescriptorSet::flush() {
    duk::hash::Hash hash = 0;
    for (auto& descriptor : m_descriptors) {
        if (descriptor.type() == DescriptorType::UNDEFINED) {
            throw std::logic_error("tried to flush a DescriptorSet with undefined descriptor");
        }
        duk::hash::hash_combine(hash, descriptor.hash());
    }
    m_descriptorSetHash = hash;
}

}