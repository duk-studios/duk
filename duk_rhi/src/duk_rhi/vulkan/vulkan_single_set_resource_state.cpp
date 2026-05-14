/// vulkan_single_set_resource_state.cpp

#include <duk_rhi/vulkan/vulkan_single_set_resource_state.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_sampler.h>
#include <duk_rhi/vulkan/vulkan_shader.h>

#include <duk_hash/hash_combine.h>

#include <stdexcept>
#include <vector>

namespace duk::rhi {

namespace detail {

static duk::hash::Hash hash_resources(const ShaderBindingLayout& layout, const ShaderResources& resources) {
    duk::hash::Hash h = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(layout.size()); i++) {
        std::visit([&h](auto&& res) {
            using T = std::decay_t<decltype(res)>;
            if constexpr (std::is_same_v<T, BufferBinding>) {
                duk::hash::hash_combine(h, reinterpret_cast<uintptr_t>(res.buffer));
            } else if constexpr (std::is_same_v<T, ImageBinding>) {
                duk::hash::hash_combine(h, reinterpret_cast<uintptr_t>(res.image));
                duk::hash::hash_combine(h, std::hash<Sampler>{}(res.sampler));
            }
        }, resources.bindings[i]);
    }
    return h;
}

}// namespace detail

VulkanSingleSetResourceState::VulkanSingleSetResourceState(
    const VulkanSingleSetResourceBinderCreateInfo& createInfo)
    : m_device(createInfo.device)
    , m_samplerCache(createInfo.samplerCache) {

    VulkanDescriptorSetLayoutCacheCreateInfo layoutCacheCreateInfo = {};
    layoutCacheCreateInfo.device = m_device;
    m_descriptorSetLayoutCache = std::make_unique<VulkanDescriptorSetLayoutCache>(layoutCacheCreateInfo);

    VulkanDescriptorSetCacheCreateInfo setCacheCreateInfo = {};
    setCacheCreateInfo.device = m_device;
    setCacheCreateInfo.maxSets = createInfo.maxDescriptorSets;
    m_descriptorSetCache = std::make_unique<VulkanDescriptorSetCache>(setCacheCreateInfo);
}

VkPipelineLayout VulkanSingleSetResourceState::pipeline_layout(const ShaderBindingLayout& bindingLayout) {
    return m_descriptorSetLayoutCache->get_pipeline_layout(bindingLayout);
}

void VulkanSingleSetResourceState::bind_resources(VkCommandBuffer commandBuffer,
                                                    VkPipelineBindPoint bindPoint,
                                                    const VulkanShader& shader,
                                                    const ShaderResources& resources,
                                                    uint32_t frameIndex) {
    const auto& bindingLayout = shader.binding_layout();
    if (bindingLayout.empty()) {
        return;
    }

    auto layout = m_descriptorSetLayoutCache->get_layout(bindingLayout);
    auto pipelineLayout = m_descriptorSetLayoutCache->get_pipeline_layout(bindingLayout);

    auto resourceHash = detail::hash_resources(bindingLayout, resources);
    duk::hash::hash_combine(resourceHash, shader.hash());

    auto [descriptorSet, isNew] = m_descriptorSetCache->get_or_allocate(layout, resourceHash);

    if (isNew) {
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo>  imageInfos;
        bufferInfos.reserve(bindingLayout.size());
        imageInfos.reserve(bindingLayout.size());

        std::vector<VkWriteDescriptorSet> writes;
        writes.reserve(bindingLayout.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(bindingLayout.size()); i++) {
            const auto& bindingDesc = bindingLayout[i];
            const auto& resource    = resources.bindings[i];

            VkWriteDescriptorSet write = {};
            write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet          = descriptorSet;
            write.dstBinding      = i;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType  = convert_descriptor_type(bindingDesc);

            bool valid = std::visit([&](auto&& res) -> bool {
                using T = std::decay_t<decltype(res)>;
                if constexpr (std::is_same_v<T, BufferBinding>) {
                    if (!res.buffer) {
                        return false;
                    }
                    auto& info  = bufferInfos.emplace_back();
                    info.buffer = static_cast<const VulkanBuffer*>(res.buffer)->handle();
                    info.offset = 0;
                    info.range  = VK_WHOLE_SIZE;
                    write.pBufferInfo = &info;
                    return true;
                } else if constexpr (std::is_same_v<T, ImageBinding>) {
                    if (!res.image) {
                        return false;
                    }
                    const auto* vulkanImage = static_cast<const VulkanImage*>(res.image);
                    auto& info       = imageInfos.emplace_back();
                    info.imageView   = vulkanImage->image_view();
                    info.imageLayout = vulkanImage->current_layout();
                    info.sampler     = m_samplerCache->get(res.sampler);
                    write.pImageInfo = &info;
                    return true;
                } else {
                    return false;
                }
            }, resource);

            if (valid) {
                writes.push_back(write);
            }
        }

        if (!writes.empty()) {
            vkUpdateDescriptorSets(m_device,
                                   static_cast<uint32_t>(writes.size()),
                                   writes.data(), 0, nullptr);
        }
    }

    vkCmdBindDescriptorSets(commandBuffer, bindPoint, pipelineLayout,
                            0, 1, &descriptorSet, 0, nullptr);
}

}// namespace duk::rhi

