//
// Created by rov on 11/17/2023.
//

#include <duk_engine/engine.h>
#include <duk_import/importer.h>

namespace duk::engine {

Engine::Engine(const EngineCreateInfo& engineCreateInfo) :
    m_logger(duk::log::Level::DEBUG),
    m_run(false) {

    m_sink.flush_from(m_logger);

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = engineCreateInfo.applicationName;
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;

    {
        auto result = duk::platform::Window::create_window(windowCreateInfo);

        if (!result) {
            throw std::runtime_error("failed to create engine window: " + result.error().description());
        }

        m_window = std::move(result.value());
    }
    m_listener.listen(m_window->window_close_event, [this] {
        m_window->close();
    });

    m_listener.listen(m_window->window_destroy_event, [this](){
        m_run = false;
    });

    duk::renderer::ForwardRendererCreateInfo forwardRendererCreateInfo = {};
    forwardRendererCreateInfo.rendererCreateInfo.window = m_window.get();
    forwardRendererCreateInfo.rendererCreateInfo.logger = &m_logger;
    forwardRendererCreateInfo.rendererCreateInfo.api = rhi::API::VULKAN;

    m_renderer = std::make_unique<duk::renderer::ForwardRenderer>(forwardRendererCreateInfo);

    duk::import::Importer::create();

    {
        ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = m_renderer.get();

        m_imagePool = std::make_unique<ImagePool>(imagePoolCreateInfo);
    }

    m_scene = std::make_unique<duk::scene::Scene>();
}

Engine::~Engine() {
    duk::import::Importer::destroy();
}

void Engine::run() {
    m_run = true;
    m_window->show();

    // assume 60fps for the first frame
    m_timer.add_duration(std::chrono::milliseconds(16));

    while (m_run) {
        m_timer.start();
        m_window->pool_events();

        for (auto& system : m_systems) {
            system->update();
        }

        m_timer.stop();
    }
}

duk::log::Logger *Engine::logger() {
    return &m_logger;
}

Systems* Engine::systems() {
    return &m_systems;
}

duk::platform::Window* Engine::window() {
    return m_window.get();
}

duk::renderer::Renderer* Engine::renderer() {
    return m_renderer.get();
}

duk::scene::Scene* Engine::scene() {
    return m_scene.get();
}

const duk::tools::Timer *Engine::timer() const {
    return &m_timer;
}

ImagePool* Engine::image_pool() {
    return m_imagePool.get();
}

}
