//
// Created by rov on 11/17/2023.
//

#include <duk_engine/engine.h>
#include <duk_import/importer.h>
#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/sprite_pool.h>
#include <duk_renderer/pools/image_pool.h>
#include <duk_log/log.h>

namespace duk::engine {

Engine::Engine(const EngineCreateInfo& engineCreateInfo) :
    m_run(false) {

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = engineCreateInfo.applicationName;
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;

    m_window = duk::platform::Window::create_window(windowCreateInfo);

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

    {
        duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);
    }

    {
        duk::renderer::MaterialPoolCreateInfo materialPoolCreateInfo = {};
        materialPoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::MaterialPool>(materialPoolCreateInfo);
    }

    {
        duk::renderer::MeshPoolCreateInfo meshPoolCreateInfo = {};
        meshPoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::MeshPool>(meshPoolCreateInfo);
    }

    {
        duk::renderer::SpritePoolCreateInfo spritePoolCreateInfo = {};
        spritePoolCreateInfo.imagePool = m_pools.get<duk::renderer::ImagePool>();
        m_pools.create_pool<duk::renderer::SpritePool>(spritePoolCreateInfo);
    }

    duk::import::ImporterCreateInfo importerCreateInfo = {};
    importerCreateInfo.renderer = m_renderer.get();
    importerCreateInfo.pools = &m_pools;

    m_importer = std::make_unique<duk::import::Importer>(importerCreateInfo);

    duk::engine::InputCreateInfo inputCreateInfo = {};
    inputCreateInfo.window = m_window.get();
    m_input = std::make_unique<duk::engine::Input>(inputCreateInfo);
}

Engine::~Engine() {
    m_importer.reset();
    m_pools.clear();
}

void Engine::run() {
    m_run = true;
    m_window->show();

    // assume 60fps for the first frame
    m_timer.add_duration(std::chrono::milliseconds(16));

    m_systems.init();

    while (m_run) {
        m_timer.start();

        m_input->refresh();

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

duk::resource::Pools* Engine::pools() {
    return &m_pools;
}

duk::import::Importer* Engine::importer() {
    return m_importer.get();
}

duk::scene::Scene* Engine::scene() {
    return m_scene;
}

duk::engine::Input* Engine::input() {
    return m_input.get();
}

const duk::tools::Timer *Engine::timer() const {
    return &m_timer;
}

void Engine::use_scene(duk::scene::Scene* scene) {
    m_scene = scene;
}

}
