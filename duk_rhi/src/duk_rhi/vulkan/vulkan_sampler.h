/// 23/04/2023
/// vulkan_sampler.h

#ifndef DUK_RHI_VULKAN_SAMPLER_H
#define DUK_RHI_VULKAN_SAMPLER_H

#include <duk_rhi/sampler.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <unordered_map>

namespace duk::rhi {

struct VulkanSamplerCacheCreateInfo {
    VkDevice device;
};

class VulkanSamplerCache {
public:
    explicit VulkanSamplerCache(const VulkanSamplerCacheCreateInfo& createInfo);

    ~VulkanSamplerCache();

    DUK_NO_DISCARD VkSampler get(const Sampler& sampler);

private:
    VkDevice m_device;
    std::unordered_map<duk::hash::Hash, VkSampler> m_samplers;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_SAMPLER_H
