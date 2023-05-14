//
// Created by Ricardo on 09/04/2023.
//

#include <duk_renderer/renderer.h>
#include <duk_renderer/vulkan/vulkan_renderer.h>

#include <sstream>

namespace duk::renderer {

static std::shared_ptr<VulkanRenderer> create_vulkan_renderer(const RendererCreateInfo& rendererCreateInfo) {
    VulkanRendererCreateInfo vulkanRendererCreateInfo = {};
    return std::make_shared<VulkanRenderer>(vulkanRendererCreateInfo);
}

Renderer::ExpectedRenderer Renderer::create_renderer(const RendererCreateInfo& rendererCreateInfo) {
    try {
        switch (rendererCreateInfo.api){
            case RendererAPI::UNDEFINED:
                return tl::unexpected<RendererError>(RendererError::INVALID_ARGUMENT, "Undefined Renderer API");
            case RendererAPI::VULKAN:
                return create_vulkan_renderer(rendererCreateInfo);
            case RendererAPI::OPENGL45:
            case RendererAPI::DX12:
                return tl::unexpected<RendererError>(RendererError::NOT_IMPLEMENTED, "Requested Renderer API is not implemented");
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
