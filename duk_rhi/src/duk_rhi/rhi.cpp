//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/rhi.h>
#include <duk_rhi/rhi_exception.h>
#include <duk_rhi/vulkan/vulkan_rhi.h>

namespace duk::rhi {

static std::shared_ptr<VulkanRHI> create_vulkan_rhi(const RHICreateInfo& rhiCreateInfo) {
    VulkanRHICreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.hasValidationLayers = rhiCreateInfo.validationLayers;
    vulkanCreateInfo.rhiCreateInfo = rhiCreateInfo;
    return std::make_shared<VulkanRHI>(vulkanCreateInfo);
}

std::shared_ptr<RHI> RHI::create_rhi(const RHICreateInfo& rhiCreateInfo) {
    switch (rhiCreateInfo.api) {
        case API::UNDEFINED:
            throw RHIException(RHIException::INVALID_ARGUMENT, "Undefined RHI API");
        case API::VULKAN:
            return create_vulkan_rhi(rhiCreateInfo);
        case API::OPENGL45:
        case API::DX12:
            throw RHIException(RHIException::NOT_IMPLEMENTED, "RHI API not implemented!");
    }

    throw RHIException(RHIException::INTERNAL_ERROR, "Unknown error occurred");
}

}// namespace duk::rhi
