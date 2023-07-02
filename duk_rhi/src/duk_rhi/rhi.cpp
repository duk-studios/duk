//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/rhi.h>
#include <duk_rhi/vulkan/vulkan_renderer.h>

#include <sstream>

namespace duk::rhi {

static std::shared_ptr<VulkanRHI> create_vulkan_rhi(const RHICreateInfo& rendererCreateInfo) {
    VulkanRHICreateInfo vulkanRendererCreateInfo = {};
    vulkanRendererCreateInfo.hasValidationLayers = true;
    vulkanRendererCreateInfo.renderHardwareInterfaceCreateInfo = rendererCreateInfo;
    vulkanRendererCreateInfo.maxFramesInFlight = 3;
    return std::make_shared<VulkanRHI>(vulkanRendererCreateInfo);
}

ExpectedRHI RHI::create_rhi(const RHICreateInfo& rendererCreateInfo) {
    try {
        switch (rendererCreateInfo.api){
            case API::UNDEFINED:
                return tl::unexpected<RendererError>(RendererError::INVALID_ARGUMENT, "Undefined RHI API");
            case API::VULKAN:
                return create_vulkan_rhi(rendererCreateInfo);
            case API::OPENGL45:
            case API::DX12:
                return tl::unexpected<RendererError>(RendererError::NOT_IMPLEMENTED, "Requested RHI API is not implemented");
        }
    }
    catch (std::exception& e) {
        std::ostringstream oss;
        oss << "Error when creating renderer: " << e.what();
        return tl::unexpected<RendererError>(RendererError::INTERNAL_ERROR, oss.str());
    }
    return tl::unexpected<RendererError>(RendererError::INTERNAL_ERROR, "Unknown error occurred");
}
}
