/// 23/04/2023
/// vulkan_sampler.h

#ifndef DUK_RENDERER_VULKAN_SAMPLER_H
#define DUK_RENDERER_VULKAN_SAMPLER_H

#include <duk_renderer/sampler.h>
#include <duk_renderer/vulkan/vulkan_import.h>

#include <unordered_map>

namespace duk::renderer {

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

}

#endif // DUK_RENDERER_VULKAN_SAMPLER_H

