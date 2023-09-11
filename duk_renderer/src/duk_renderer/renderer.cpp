/// 19/08/2023
/// renderer.cpp

#include <duk_renderer/renderer.h>
#include <duk_platform/window.h>

namespace duk::renderer {

Renderer::Renderer(const RendererCreateInfo& rendererCreateInfo) :
    m_window(rendererCreateInfo.window) {

    {
        duk::rhi::RHICreateInfo rhiCreateInfo = {};
        rhiCreateInfo.window = rendererCreateInfo.window;
        rhiCreateInfo.logger = rendererCreateInfo.logger;
        rhiCreateInfo.deviceIndex = 0;
        rhiCreateInfo.applicationVersion = 1;
        rhiCreateInfo.applicationName = "duk_application";
        rhiCreateInfo.engineVersion = 1;
        rhiCreateInfo.engineName = "duk_engine";
        rhiCreateInfo.api = rendererCreateInfo.api;

        auto expectedRHI = duk::rhi::RHI::create_rhi(rhiCreateInfo);

        if (!expectedRHI) {
            throw std::runtime_error("failed to create RHI: " + expectedRHI.error().description());
        }

        m_rhi = std::move(expectedRHI.value());
    }


    {
        duk::rhi::RHI::CommandQueueCreateInfo mainCommandQueueCreateInfo = {};
        mainCommandQueueCreateInfo.type = rhi::CommandQueue::Type::GRAPHICS;

        auto expectedCommandQueue = m_rhi->create_command_queue(mainCommandQueueCreateInfo);

        if (!expectedCommandQueue) {
            throw std::runtime_error("failed to create main CommandQueue: " + expectedCommandQueue.error().description());
        }

        m_mainQueue = std::move(expectedCommandQueue.value());
    }

    {
        auto expectedScheduler = m_rhi->create_command_scheduler();

        if (!expectedScheduler) {
            throw std::runtime_error("failed to create CommandScheduler: " + expectedScheduler.error().description());
        }

        m_scheduler = std::move(expectedScheduler.value());
    }

}

void Renderer::resize(uint32_t width, uint32_t height) {

}

uint32_t Renderer::render_width() const {
    return m_window ? m_window->width() : 0;
}

uint32_t Renderer::render_height() const {
    return m_window ? m_window->height() : 0;
}

std::shared_ptr<duk::rhi::Image> Renderer::create_depth_image(uint32_t width, uint32_t height) {
    duk::rhi::EmptyImageDataSource depthImageDataSource(width, height, m_rhi->capabilities()->depth_format());
    depthImageDataSource.update_hash();

    duk::rhi::RHI::ImageCreateInfo depthImageCreateInfo = {};
    depthImageCreateInfo.usage = duk::rhi::Image::Usage::DEPTH_STENCIL_ATTACHMENT;
    depthImageCreateInfo.initialLayout = duk::rhi::Image::Layout::DEPTH_ATTACHMENT;
    depthImageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::DEVICE_DYNAMIC;
    depthImageCreateInfo.imageDataSource = &depthImageDataSource;
    depthImageCreateInfo.commandQueue = m_mainQueue.get();

    auto expectedDepthImage = m_rhi->create_image(depthImageCreateInfo);

    if (!expectedDepthImage) {
        throw std::runtime_error("failed to create depth image: " + expectedDepthImage.error().description());
    }

    return std::move(expectedDepthImage.value());
}

duk::rhi::RHI* Renderer::rhi() const {
    return m_rhi.get();
}

duk::rhi::CommandQueue* Renderer::main_command_queue() const {
    return m_mainQueue.get();
}

}
