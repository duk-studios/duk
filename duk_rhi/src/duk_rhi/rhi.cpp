//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/rhi.h>
#include <duk_rhi/rhi_exception.h>
#include <duk_rhi/vulkan/vulkan_rhi.h>

namespace duk::rhi {

static std::shared_ptr<VulkanRHI> create_vulkan_rhi(const RHICreateInfo& rendererCreateInfo) {
    VulkanRHICreateInfo vulkanRendererCreateInfo = {};
    vulkanRendererCreateInfo.hasValidationLayers = true;
    vulkanRendererCreateInfo.renderHardwareInterfaceCreateInfo = rendererCreateInfo;
    vulkanRendererCreateInfo.maxFramesInFlight = 3;
    return std::make_shared<VulkanRHI>(vulkanRendererCreateInfo);
}

std::shared_ptr<RHI> RHI::create_rhi(const RHICreateInfo& rendererCreateInfo) {
    switch (rendererCreateInfo.api) {
        case API::UNDEFINED:
            throw RHIException(RHIException::INVALID_ARGUMENT, "Undefined RHI API");
        case API::VULKAN:
            return create_vulkan_rhi(rendererCreateInfo);
        case API::OPENGL45:
        case API::DX12:
            throw RHIException(RHIException::NOT_IMPLEMENTED, "RHI API not implemented!");
    }

    throw RHIException(RHIException::INTERNAL_ERROR, "Unknown error occurred");
}
}// namespace duk::rhi
