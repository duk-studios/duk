/// 19/08/2023
/// renderer.cpp

#include <duk_platform/window.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/passes/pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/sprite/sprite_cache.h>
#include <duk_renderer/builtins.h>

namespace duk::renderer {

Renderer::Renderer(const RendererCreateInfo& rendererCreateInfo)
    : m_window(rendererCreateInfo.window) {
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
        rhiCreateInfo.validationLayers = rendererCreateInfo.apiValidationLayers;

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

        m_meshBufferPool = std::make_unique<MeshBufferPool>(meshBufferPoolCreateInfo);
    }
    {
        PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.renderer = this;
        m_pipelineCache = std::make_unique<PipelineCache>(pipelineCacheCreateInfo);
    }
    m_spriteCache = std::make_unique<SpriteCache>();
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

duk::rhi::RHI* Renderer::rhi() const {
    return m_rhi.get();
}

duk::rhi::CommandQueue* Renderer::main_command_queue() const {
    return m_mainQueue.get();
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

    m_globalDescriptors->update_lights(objects);
}

void Renderer::update_passes(objects::Objects& objects) {
    Pass::UpdateParams updateParams = {};
    updateParams.objects = &objects;
    updateParams.globalDescriptors = m_globalDescriptors.get();
    updateParams.viewport = {render_width(), render_height()};
    updateParams.pipelineCache = m_pipelineCache.get();

    for (auto& pass: m_passes) {
        pass->update(updateParams);
    }
}

}// namespace duk::renderer
