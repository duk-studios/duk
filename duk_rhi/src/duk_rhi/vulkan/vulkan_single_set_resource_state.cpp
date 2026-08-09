/// vulkan_single_set_resource_state.cpp

#include <duk_rhi/vulkan/vulkan_single_set_resource_state.h>
#include <duk_rhi/vulkan/vulkan_buffer.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_sampler.h>
#include <duk_rhi/vulkan/vulkan_shader.h>

#include <duk_hash/hash_combine.h>
#include <duk_macros/assert.h>

#include <stdexcept>
#include <vector>

namespace duk::rhi {

namespace detail {

static duk::hash::Hash hash_resources(const BindingLayout& layout, const ShaderBindings& resources) {
    duk::hash::Hash h = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(layout.size()); i++) {
        std::visit(
                [&h](auto&& res) {
                    using T = std::decay_t<decltype(res)>;
                    if constexpr (std::is_same_v<T, BufferResource>) {
                        // All buffers are dynamic: offset is supplied at bind time via the
                        // dynamic offset array and must NOT be in the hash — the same
                        // descriptor set is reused for every offset into the same buffer.
                        duk::hash::hash_combine(h, reinterpret_cast<uintptr_t>(res.buffer));
                    } else if constexpr (std::is_same_v<T, ImageResource>) {
                        duk::hash::hash_combine(h, reinterpret_cast<uintptr_t>(res.image));
                        duk::hash::hash_combine(h, std::hash<Sampler>{}(res.sampler));
                    }
                },
                resources.resources[i]);
    }
    return h;
}

}// namespace detail

VulkanSingleSetResourceState::VulkanSingleSetResourceState(const VulkanSingleSetResourceBinderCreateInfo& createInfo)
    : m_device(createInfo.device)
    , m_samplerCache(createInfo.samplerCache)
    , m_uniformBufferOffsetAlignment(createInfo.physicalDevice->properties().limits.minUniformBufferOffsetAlignment)
    , m_storageBufferOffsetAlignment(createInfo.physicalDevice->properties().limits.minStorageBufferOffsetAlignment) {
    VulkanDescriptorSetLayoutCacheCreateInfo layoutCacheCreateInfo = {};
    layoutCacheCreateInfo.device = m_device;
    m_descriptorSetLayoutCache = std::make_unique<VulkanDescriptorSetLayoutCache>(layoutCacheCreateInfo);

    VulkanDescriptorSetCacheCreateInfo setCacheCreateInfo = {};
    setCacheCreateInfo.device = m_device;
    setCacheCreateInfo.maxSets = createInfo.maxDescriptorSets;
    m_descriptorSetCache = std::make_unique<VulkanDescriptorSetCache>(setCacheCreateInfo);
}

VkPipelineLayout VulkanSingleSetResourceState::pipeline_layout(const BindingLayout& bindingLayout) {
    return m_descriptorSetLayoutCache->get_pipeline_layout(bindingLayout);
}

void VulkanSingleSetResourceState::bind_resources(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, const VulkanShader& shader, const ShaderBindings& resources, uint32_t frameIndex) {
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
        std::vector<VkDescriptorImageInfo> imageInfos;
        bufferInfos.reserve(bindingLayout.size());
        imageInfos.reserve(bindingLayout.size());

        std::vector<VkWriteDescriptorSet> writes;
        writes.reserve(bindingLayout.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(bindingLayout.size()); i++) {
            const auto& bindingDesc = bindingLayout[i];
            const auto& resource = resources.resources[i];

            VkWriteDescriptorSet write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSet;
            write.dstBinding = i;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = convert_descriptor_type(bindingDesc);

            bool valid = std::visit(
                    [&](auto&& res) -> bool {
                        using T = std::decay_t<decltype(res)>;
                        if constexpr (std::is_same_v<T, BufferResource>) {
                            if (!res.buffer) {
                                return false;
                            }
                            auto& info = bufferInfos.emplace_back();
                            info.buffer = static_cast<const VulkanBuffer*>(res.buffer)->handle();
                            // offset is always 0: the actual byte offset is supplied at bind
                            // time via the dynamic offset array (see below).
                            info.offset = 0;
                            // Clamp the range to the actual buffer size so that validation passes
                            // when the buffer was allocated smaller than the reflection-reported size
                            // (which may still be padded by the driver/compiler).
                            info.range = std::min<VkDeviceSize>(bindingDesc.size, res.buffer->size());
                            write.pBufferInfo = &info;
                            return true;
                        } else if constexpr (std::is_same_v<T, ImageResource>) {
                            if (!res.image) {
                                return false;
                            }
                            const auto* vulkanImage = static_cast<const VulkanImage*>(res.image);
                            auto& info = imageInfos.emplace_back();
                            info.imageView = vulkanImage->image_view();
                            info.imageLayout = vulkanImage->current_layout();
                            info.sampler = m_samplerCache->get(res.sampler);
                            write.pImageInfo = &info;
                            return true;
                        } else {
                            return false;
                        }
                    },
                    resource);

            if (valid) {
                writes.push_back(write);
            }
        }

        if (!writes.empty()) {
            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
        }
    }

    // Collect dynamic offsets in binding-index order.
    // Every buffer slot uses a *_DYNAMIC descriptor type, so Vulkan requires
    // exactly one dynamic offset entry per buffer binding.
    // The offset must satisfy the physical device's minimum alignment requirement.
    std::vector<uint32_t> dynamicOffsets;
    for (uint32_t i = 0; i < static_cast<uint32_t>(bindingLayout.size()); i++) {
        if (bindingLayout[i].type != BindingType::UNIFORM_BUFFER && bindingLayout[i].type != BindingType::STORAGE_BUFFER) {
            continue;
        }

        const bool isUniform = bindingLayout[i].type == BindingType::UNIFORM_BUFFER;
        const VkDeviceSize alignment = isUniform ? m_uniformBufferOffsetAlignment : m_storageBufferOffsetAlignment;

        uint32_t offset = 0;
        if (const auto* bb = std::get_if<BufferResource>(&resources.resources[i])) {
            // Assert device alignment requirements in debug builds.
            DUK_ASSERT(alignment == 0 || (bb->offset % alignment) == 0);
            offset = bb->offset;
        }
        dynamicOffsets.push_back(offset);
    }

    vkCmdBindDescriptorSets(commandBuffer, bindPoint, pipelineLayout, 0, 1, &descriptorSet, static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.empty() ? nullptr : dynamicOffsets.data());
}

}// namespace duk::rhi
