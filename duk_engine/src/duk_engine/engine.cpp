//
// Created by rov on 11/17/2023.
//

#include <duk_engine/engine.h>
#include <duk_engine/systems/render_system.h>
#include <duk_import/image/image_importer.h>
#include <duk_import/importer.h>
#include <duk_import/material/material_importer.h>
#include <duk_import/scene/scene_importer.h>
#include <duk_log/log.h>
#include <duk_renderer/pools/image_pool.h>
#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/sprite_pool.h>

namespace duk::engine {

Engine::Engine(const EngineCreateInfo& engineCreateInfo)
    : m_run(false) {
    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = engineCreateInfo.applicationName;
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;

    m_window = duk::platform::Window::create_window(windowCreateInfo);

    m_listener.listen(m_window->window_close_event, [this] {
        m_window->close();
    });

    m_listener.listen(m_window->window_destroy_event, [this]() {
        m_run = false;
    });

    duk::renderer::ForwardRendererCreateInfo forwardRendererCreateInfo = {};
    forwardRendererCreateInfo.rendererCreateInfo.window = m_window.get();
    forwardRendererCreateInfo.rendererCreateInfo.logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::DEBUG));
    forwardRendererCreateInfo.rendererCreateInfo.api = duk::rhi::API::VULKAN;

    m_renderer = std::make_unique<duk::renderer::ForwardRenderer>(forwardRendererCreateInfo);

    duk::import::ImporterCreateInfo importerCreateInfo = {};
    importerCreateInfo.pools = &m_pools;
    m_importer = std::make_unique<duk::import::Importer>(importerCreateInfo);

    /* init resources */
    // images
    {
        duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);

        duk::import::ImageImporterCreateInfo imageImporterCreateInfo = {};
        imageImporterCreateInfo.rhiCapabilities = m_renderer->rhi()->capabilities();
        imageImporterCreateInfo.imagePool = m_pools.get<duk::renderer::ImagePool>();
        m_importer->add_resource_importer<duk::import::ImageImporter>(imageImporterCreateInfo);
    }

    // materials
    {
        duk::renderer::MaterialPoolCreateInfo materialPoolCreateInfo = {};
        materialPoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::MaterialPool>(materialPoolCreateInfo);

        duk::import::MaterialImporterCreateInfo materialImporterCreateInfo = {};
        materialImporterCreateInfo.materialPool = m_pools.get<duk::renderer::MaterialPool>();
        m_importer->add_resource_importer<duk::import::MaterialImporter>(materialImporterCreateInfo);
    }

    // meshes
    {
        duk::renderer::MeshPoolCreateInfo meshPoolCreateInfo = {};
        meshPoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::MeshPool>(meshPoolCreateInfo);
    }

    // sprites
    {
        duk::renderer::SpritePoolCreateInfo spritePoolCreateInfo = {};
        spritePoolCreateInfo.imagePool = m_pools.get<duk::renderer::ImagePool>();
        m_pools.create_pool<duk::renderer::SpritePool>(spritePoolCreateInfo);
    }

    // scenes
    {
        m_importer->add_resource_importer<duk::import::SceneImporter>();
    }

    duk::scene::register_system<RenderSystem>(*this);

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

    auto& systems = m_scene->systems();

    systems.enter();

    while (m_run) {
        m_timer.start();

        m_input->refresh();

        m_window->pool_events();

        systems.update();

        m_timer.stop();
    }

    systems.exit();
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

const duk::engine::Input* Engine::input() const {
    return m_input.get();
}

const duk::tools::Timer* Engine::timer() const {
    return &m_timer;
}

void Engine::use_scene(duk::scene::Scene* scene) {
    m_scene = scene;
}

}// namespace duk::engine
