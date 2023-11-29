/// 19/08/2023
/// renderer.cpp

#include <duk_renderer/renderer.h>
#include <duk_renderer/passes/pass.h>
#include <duk_renderer/materials/globals/global_descriptors.h>

#include <duk_renderer/pools/image_pool.h>
#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/material_pool.h>

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

    {
        GlobalDescriptorsCreateInfo globalDescriptorsCreateInfo = {};
        globalDescriptorsCreateInfo.rhi = m_rhi.get();
        globalDescriptorsCreateInfo.commandQueue = m_mainQueue.get();

        m_globalDescriptors = std::make_unique<GlobalDescriptors>(globalDescriptorsCreateInfo);
    }

    {
        ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = this;

        m_imagePool = std::make_unique<ImagePool>(imagePoolCreateInfo);
    }

    {
        MaterialPoolCreateInfo materialPoolCreateInfo = {};
        materialPoolCreateInfo.renderer = this;
        materialPoolCreateInfo.imagePool = m_imagePool.get();

        m_materialPool = std::make_unique<MaterialPool>(materialPoolCreateInfo);
    }

    {
        MeshPoolCreateInfo meshPoolCreateInfo = {};
        meshPoolCreateInfo.renderer = this;

        m_meshPool = std::make_unique<MeshPool>(meshPoolCreateInfo);
    }
}

Renderer::~Renderer() = default;

void Renderer::render(duk::scene::Scene* scene) {
    // if no camera is set, return
    if (!scene->valid_object(m_mainCameraObjectId)) {
        return;
    }

    m_rhi->prepare_frame();

    m_scheduler->begin();

    m_renderStart.emit();

    update_global_descriptors(scene);

    auto mainPass = m_mainQueue->submit([this, scene](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        Pass::RenderParams renderParams = {};
        renderParams.commandBuffer = commandBuffer;
        renderParams.scene = scene;
        renderParams.globalDescriptors = m_globalDescriptors.get();
        renderParams.outputWidth = render_width();
        renderParams.outputHeight = render_height();

        for (auto& pass : m_passes) {
            pass->render(renderParams);
        }

        commandBuffer->end();
    });

    auto acquireImageCommand = m_scheduler->schedule(m_rhi->acquire_image_command());
    auto mainPassCommand = m_scheduler->schedule(std::move(mainPass));
    auto presentCommand = m_scheduler->schedule(m_rhi->present_command());

    mainPassCommand.wait(acquireImageCommand, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT);

    presentCommand.wait(mainPassCommand);

    m_scheduler->flush();
}

uint32_t Renderer::render_width() const {
    return m_window ? m_window->width() : 0;
}

uint32_t Renderer::render_height() const {
    return m_window ? m_window->height() : 0;
}

std::shared_ptr<duk::rhi::Image> Renderer::create_depth_image(uint32_t width, uint32_t height) {
    duk::rhi::ImageDataSourceEmpty depthImageDataSource(width, height, m_rhi->capabilities()->depth_format());
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

std::shared_ptr<duk::rhi::Image> Renderer::create_color_image(uint32_t width, uint32_t height, duk::rhi::PixelFormat format) {
    duk::rhi::ImageDataSourceEmpty colorImageDataSource(width, height, format);
    colorImageDataSource.update_hash();

    duk::rhi::RHI::ImageCreateInfo colorImageCreateInfo = {};
    colorImageCreateInfo.usage = duk::rhi::Image::Usage::COLOR_ATTACHMENT;
    colorImageCreateInfo.initialLayout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
    colorImageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::DEVICE_DYNAMIC;
    colorImageCreateInfo.imageDataSource = &colorImageDataSource;
    colorImageCreateInfo.commandQueue = m_mainQueue.get();

    auto expectedColorImage = m_rhi->create_image(colorImageCreateInfo);

    if (!expectedColorImage) {
        throw std::runtime_error("failed to create color image: " + expectedColorImage.error().description());
    }

    return std::move(expectedColorImage.value());
}

duk::rhi::RHI* Renderer::rhi() const {
    return m_rhi.get();
}

duk::rhi::CommandQueue* Renderer::main_command_queue() const {
    return m_mainQueue.get();
}

Renderer::RenderStartEvent& Renderer::render_start_event() {
    return m_renderStart;
}

ImagePool* Renderer::image_pool() {
    return m_imagePool.get();
}

MeshPool* Renderer::mesh_pool() {
    return m_meshPool.get();
}

MaterialPool* Renderer::material_pool() {
    return m_materialPool.get();
}

void Renderer::use_as_camera(const scene::Object& object) {
    m_mainCameraObjectId = object.id();
}

void Renderer::update_global_descriptors(duk::scene::Scene* scene) {
    if (scene->valid_object(m_mainCameraObjectId)) {
        m_globalDescriptors->update_camera(scene->object(m_mainCameraObjectId));
    }

    m_globalDescriptors->update_lights(scene);
}

}
