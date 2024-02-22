/// 23/04/2023
/// vulkan_sampler.cpp

#include <duk_rhi/vulkan/vulkan_sampler.h>

#include <stdexcept>

namespace duk::rhi {

namespace detail {

duk::hash::Hash calculate_hash(const Sampler& sampler) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, sampler);
    return hash;
}

VkSamplerAddressMode convert_wrap_mode(Sampler::WrapMode wrapMode) {
    VkSamplerAddressMode converted;
    switch (wrapMode) {
        case Sampler::WrapMode::REPEAT:
            converted = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            break;
        case Sampler::WrapMode::MIRRORED_REPEAT:
            converted = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            break;
        case Sampler::WrapMode::CLAMP_TO_EDGE:
            converted = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;
        case Sampler::WrapMode::CLAMP_TO_BORDER:
            converted = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            break;
        case Sampler::WrapMode::MIRROR_CLAMP_TO_EDGE:
            converted = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            break;
        default:
            throw std::invalid_argument("unhandled Sampler::WrapMode conversion");
    }
    return converted;
}

VkFilter convert_filter(Sampler::Filter filter) {
    VkFilter converted;
    switch (filter) {
        case Sampler::Filter::NEAREST:
            converted = VK_FILTER_NEAREST;
            break;
        case Sampler::Filter::LINEAR:
            converted = VK_FILTER_LINEAR;
            break;
        case Sampler::Filter::CUBIC:
            converted = VK_FILTER_CUBIC_EXT;
            break;
        default:
            throw std::logic_error("unhandled Sampler::Filter conversion");
    }
    return converted;
}

}// namespace detail

VulkanSamplerCache::VulkanSamplerCache(const VulkanSamplerCacheCreateInfo& imageSamplerCacheCreateInfo) : m_device(imageSamplerCacheCreateInfo.device) {
}

VulkanSamplerCache::~VulkanSamplerCache() {
    for (auto [hash, sampler]: m_samplers) {
        vkDestroySampler(m_device, sampler, nullptr);
    }
}

VkSampler VulkanSamplerCache::get(const Sampler& sampler) {
    auto hash = detail::calculate_hash(sampler);
    {
        auto it = m_samplers.find(hash);
        if (it != m_samplers.end()) {
            return it->second;
        }
    }

    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    auto filter = detail::convert_filter(sampler.filter);
    samplerCreateInfo.magFilter = filter;
    samplerCreateInfo.minFilter = filter;
    auto addressMode = detail::convert_wrap_mode(sampler.wrapMode);
    samplerCreateInfo.addressModeU = addressMode;
    samplerCreateInfo.addressModeV = addressMode;
    samplerCreateInfo.addressModeW = addressMode;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 1;

    VkSampler vkSampler;

    if (vkCreateSampler(m_device, &samplerCreateInfo, nullptr, &vkSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkSampler");
    }

    auto [it, inserted] = m_samplers.emplace(hash, vkSampler);

    if (!inserted) {
        throw std::runtime_error("failed to cache VkSampler");
    }

    return it->second;
}
}// namespace duk::rhi