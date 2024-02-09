//
// Created by rov on 11/17/2023.
//

#include <duk_engine/engine.h>
#include <duk_import/importer.h>
#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/sprite_pool.h>
#include <duk_log/log.h>
#include <duk_renderer/components/register_components.h>

namespace duk::engine {

Engine::Engine(const EngineCreateInfo& engineCreateInfo) :
    m_run(false) {

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
    forwardRendererCreateInfo.rendererCreateInfo.logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::DEBUG));
    forwardRendererCreateInfo.rendererCreateInfo.api = duk::rhi::API::VULKAN;

    m_renderer = std::make_unique<duk::renderer::ForwardRenderer>(forwardRendererCreateInfo);

    m_solver.add_pool(m_renderer->image_pool());
    m_solver.add_pool(m_renderer->mesh_pool());
    m_solver.add_pool(m_renderer->sprite_pool());
    m_solver.add_pool(m_renderer->material_pool());

    {
        duk::scene::ComponentBuilderCreateInfo componentBuilderCreateInfo = {};
        componentBuilderCreateInfo.solver = &m_solver;
        m_componentBuilder = std::make_unique<duk::scene::ComponentBuilder>(componentBuilderCreateInfo);
    }

    duk::renderer::register_components(m_componentBuilder.get());

    duk::import::ImporterCreateInfo importerCreateInfo = {};
    importerCreateInfo.renderer = m_renderer.get();
    importerCreateInfo.componentBuilder = m_componentBuilder.get();

    m_importer = std::make_unique<duk::import::Importer>(importerCreateInfo);
}

Engine::~Engine() = default;

void Engine::run() {
    m_run = true;
    m_window->show();

    // assume 60fps for the first frame
    m_timer.add_duration(std::chrono::milliseconds(16));

    m_systems.init();

    while (m_run) {
        m_timer.start();
        m_window->pool_events();

        for (auto& system : m_systems) {
            system->update();
        }

        m_timer.stop();
    }
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

duk::import::Importer* Engine::importer() {
    return m_importer.get();
}

duk::scene::Scene* Engine::scene() {
    return m_scene;
}

const duk::tools::Timer *Engine::timer() const {
    return &m_timer;
}

void Engine::use_scene(duk::scene::Scene* scene) {
    m_scene = scene;
}

}
