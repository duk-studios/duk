/// 19/08/2023
/// renderer.cpp

#include <duk_platform/window.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/passes/pass.h>
#include <duk_renderer/passes/present_pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/sprite/sprite_cache.h>
#include <duk_rhi/image_data_source.h>

namespace duk::renderer {

Renderer::Renderer(const RendererCreateInfo& rendererCreateInfo)
    : m_window(rendererCreateInfo.window)
    , m_pools(rendererCreateInfo.pools) {
    {
        duk::rhi::RHICreateInfo rhiCreateInfo = {};
        rhiCreateInfo.window = rendererCreateInfo.window;
        rhiCreateInfo.logger = rendererCreateInfo.logger;
        rhiCreateInfo.deviceIndex = 0;
        rhiCreateInfo.applicationVersion = 1;
        rhiCreateInfo.applicationName = rendererCreateInfo.applicationName;
        rhiCreateInfo.engineVersion = 1;
        rhiCreateInfo.engineName = "duk::renderer::Renderer::m_rhi";
        rhiCreateInfo.api = rendererCreateInfo.api;

        m_rhi = duk::rhi::RHI::create_rhi(rhiCreateInfo);
    }

    duk::rhi::RHI::CommandQueueCreateInfo mainCommandQueueCreateInfo = {};
    mainCommandQueueCreateInfo.type = rhi::CommandQueue::Type::GRAPHICS;

    m_mainQueue = m_rhi->create_command_queue(mainCommandQueueCreateInfo);

    {
        GlobalDescriptorsCreateInfo globalDescriptorsCreateInfo = {};
        globalDescriptorsCreateInfo.rhi = m_rhi.get();
        globalDescriptorsCreateInfo.commandQueue = m_mainQueue.get();

        m_globalDescriptors = std::make_unique<GlobalDescriptors>(globalDescriptorsCreateInfo);
    }
    {
        duk::renderer::MeshBufferPoolCreateInfo meshBufferPoolCreateInfo = {};
        meshBufferPoolCreateInfo.rhi = m_rhi.get();
        meshBufferPoolCreateInfo.commandQueue = m_mainQueue.get();

        m_meshBufferPool = std::make_unique<renderer::MeshBufferPool>(meshBufferPoolCreateInfo);
    }
    {
        PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.renderer = this;
        m_pipelineCache = std::make_unique<PipelineCache>(pipelineCacheCreateInfo);
    }
    {
        SpriteCacheCreateInfo spriteCacheCreateInfo = {};
        spriteCacheCreateInfo.renderer = this;

        m_spriteCache = std::make_unique<SpriteCache>(spriteCacheCreateInfo);
    }
}

Renderer::~Renderer() = default;

void Renderer::render(duk::objects::Objects& objects) {
    // if rendering to a window, but it's minimized, skip
    if (m_window && m_window->minimized()) {
        return;
    }

    m_rhi->prepare_frame();

    update_global_descriptors(objects);

    update_passes(objects);

    m_rhi->update();

    auto mainPass = m_mainQueue->submit([this](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        for (auto& pass: m_passes) {
            pass->render(commandBuffer);
        }

        commandBuffer->end();
    });

    auto scheduler = m_rhi->command_scheduler();

    auto acquireImageCommand = scheduler->schedule(m_rhi->acquire_image_command());
    auto mainPassCommand = scheduler->schedule(std::move(mainPass));
    auto presentCommand = scheduler->schedule(m_rhi->present_command());

    mainPassCommand.wait(acquireImageCommand, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT);

    presentCommand.wait(mainPassCommand);

    scheduler->flush();
}

void Renderer::clear_cache() {
    m_pipelineCache->clear();
    m_spriteCache->clear();
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
    depthImageCreateInfo.dstStages = duk::rhi::PipelineStage::EARLY_FRAGMENT_TESTS;

    auto expectedDepthImage = m_rhi->create_image(depthImageCreateInfo);

    if (!expectedDepthImage) {
        throw std::runtime_error("failed to create depth image!");
    }

    return expectedDepthImage;
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
    colorImageCreateInfo.dstStages = duk::rhi::PipelineStage::FRAGMENT_SHADER | duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT;

    auto expectedColorImage = m_rhi->create_image(colorImageCreateInfo);

    if (!expectedColorImage) {
        throw std::runtime_error("failed to create color image!");
    }

    return expectedColorImage;
}

duk::rhi::RHI* Renderer::rhi() const {
    return m_rhi.get();
}

duk::rhi::CommandQueue* Renderer::main_command_queue() const {
    return m_mainQueue.get();
}

duk::resource::Pools* Renderer::pools() const {
    return m_pools;
}

GlobalDescriptors* Renderer::global_descriptors() const {
    return m_globalDescriptors.get();
}

MeshBufferPool* Renderer::mesh_buffer_pool() const {
    return m_meshBufferPool.get();
}

SpriteCache* Renderer::sprite_cache() const {
    return m_spriteCache.get();
}

void Renderer::update_global_descriptors(duk::objects::Objects& objects) {
    m_globalDescriptors->update_cameras(objects);

    m_globalDescriptors->update_canvas(objects);

    m_globalDescriptors->update_lights(objects);
}

void Renderer::update_passes(objects::Objects& objects) {
    Pass::UpdateParams updateParams = {};
    updateParams.objects = &objects;
    updateParams.globalDescriptors = m_globalDescriptors.get();
    updateParams.viewport = {render_width(), render_height()};
    updateParams.pipelineCache = m_pipelineCache.get();
    updateParams.spriteCache = m_spriteCache.get();

    for (auto& pass: m_passes) {
        pass->update(updateParams);
    }
}

std::unique_ptr<Renderer> make_forward_renderer(const RendererCreateInfo& rendererCreateInfo) {
    auto renderer = std::make_unique<Renderer>(rendererCreateInfo);

    ForwardPassCreateInfo forwardPassCreateInfo = {};
    forwardPassCreateInfo.renderer = renderer.get();

    auto forwardPass = renderer->add_pass<ForwardPass>(forwardPassCreateInfo);

    if (rendererCreateInfo.window) {
        PresentPassCreateInfo presentPassCreateInfo = {};
        presentPassCreateInfo.renderer = renderer.get();
        presentPassCreateInfo.window = rendererCreateInfo.window;

        auto presentPass = renderer->add_pass<PresentPass>(presentPassCreateInfo);

        forwardPass->out_color()->connect(presentPass->in_color());
    }

    return renderer;
}

}// namespace duk::renderer
