//
// Created by rov on 11/17/2023.
//

#include <duk_engine/engine.h>
#include <duk_engine/scene/scene_handler.h>

#include <duk_objects/objects_handler.h>

#include <duk_audio/clip/audio_clip_handler.h>

#include <duk_renderer/font/font_handler.h>
#include <duk_renderer/material/material_handler.h>
#include <duk_renderer/mesh/mesh_pool.h>
#include <duk_renderer/sprite/sprite_pool.h>
#include <duk_renderer/shader/shader_module_pool.h>
#include <duk_renderer/shader/shader_pipeline_pool.h>

#include <duk_log/log.h>

#include <duk_platform/platform.h>

namespace duk::engine {

Engine::Engine(const EngineCreateInfo& engineCreateInfo)
    : m_workingDirectory(engineCreateInfo.workingDirectory)
    , m_platform(engineCreateInfo.platform)
    , m_window(engineCreateInfo.window)
    , m_run(false) {
    const auto& settings = engineCreateInfo.settings;

    m_listener.listen(m_window->window_close_event, [this] {
        m_window->close();
    });

    m_listener.listen(m_window->window_destroy_event, [this]() {
        m_run = false;
    });

    m_pools = m_globals.make<duk::resource::Pools>();

    {
        duk::renderer::RendererCreateInfo rendererCreateInfo = {};
        rendererCreateInfo.window = m_window;
        rendererCreateInfo.pools = m_pools;
        rendererCreateInfo.logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::VERBOSE));
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.applicationName = settings.name.c_str();

        m_renderer = m_globals.make<duk::renderer::Renderer>(rendererCreateInfo);
        duk::renderer::add_forward_passes(m_renderer, m_window);
    }

    {
        duk::audio::AudioEngineCreateInfo audioEngineCreateInfo = {};
        audioEngineCreateInfo.channelCount = 2;
        audioEngineCreateInfo.frameRate = 48000;

        m_audio = m_globals.make<duk::audio::AudioEngine>(audioEngineCreateInfo);
    }

    {
        duk::resource::ResourceSetCreateInfo resourceSetCreateInfo = {};
        resourceSetCreateInfo.path = m_workingDirectory / "resources";
        resourceSetCreateInfo.loadMode = settings.loadMode;
        resourceSetCreateInfo.pools = m_pools;
        m_resources = m_globals.make<duk::resource::ResourceSet>(resourceSetCreateInfo);
    }

    /* init pools */
    {
        duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = m_renderer;
        m_pools->create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);

        auto shaderModulePool = m_pools->create_pool<duk::renderer::ShaderModulePool>();

        duk::renderer::ShaderPipelinePoolCreateInfo shaderPipelinePoolCreateInfo = {};
        shaderPipelinePoolCreateInfo.renderer = m_renderer;
        shaderPipelinePoolCreateInfo.shaderModulePool = shaderModulePool;
        m_pools->create_pool<duk::renderer::ShaderPipelinePool>(shaderPipelinePoolCreateInfo);

        duk::renderer::MaterialPoolCreateInfo materialPoolCreateInfo = {};
        materialPoolCreateInfo.renderer = m_renderer;
        m_pools->create_pool<duk::renderer::MaterialPool>(materialPoolCreateInfo);

        duk::renderer::MeshPoolCreateInfo meshPoolCreateInfo = {};
        meshPoolCreateInfo.meshBufferPool = m_renderer->mesh_buffer_pool();
        m_pools->create_pool<duk::renderer::MeshPool>(meshPoolCreateInfo);
        m_pools->create_pool<duk::renderer::SpritePool>();

        duk::audio::AudioClipPoolCreateInfo audioClipPoolCreateInfo = {};
        audioClipPoolCreateInfo.engine = m_audio;
        m_pools->create_pool<duk::audio::AudioClipPool>(audioClipPoolCreateInfo);

        m_pools->create_pool<duk::renderer::FontPool>();
        m_pools->create_pool<duk::objects::ObjectsPool>();
        m_pools->create_pool<ScenePool>();
    }

    // director
    {
        DirectorCreateInfo directorCreateInfo = {};
        directorCreateInfo.renderer = m_renderer;
        directorCreateInfo.resources = m_resources;
        directorCreateInfo.firstScene = settings.scene;

        m_director = m_globals.make<Director>(directorCreateInfo);
    }

    duk::engine::InputCreateInfo inputCreateInfo = {};
    inputCreateInfo.window = m_window;
    m_input = m_globals.make<Input>(inputCreateInfo);

    m_timer = m_globals.make<duk::tools::Timer>();
    m_dispatcher = m_globals.make<duk::event::Dispatcher>();
}

Engine::~Engine() {
    m_globals.reset<Director>();
    m_pools->clear();
    m_globals.reset<duk::resource::ResourceSet>();
}

void Engine::run() {
    m_run = true;

    m_window->show();

    m_audio->start();

    // assume 60fps for the first frame
    m_timer->add_duration(std::chrono::milliseconds(16));

    while (m_run) {
        m_timer->tick();

        m_input->refresh();

        m_platform->pool_events();

        while (m_window->valid() && m_window->minimized()) {
            m_platform->wait_events();
        }

        if (!m_window->valid()) {
            continue;
        }

        m_audio->update();

        m_director->update(*this);
    }
}

duk::platform::Platform* Engine::platform() {
    return m_platform;
}

duk::platform::Window* Engine::window() {
    return m_window;
}

duk::renderer::Renderer* Engine::renderer() {
    return m_renderer;
}

duk::audio::AudioEngine* Engine::audio() {
    return m_audio;
}

duk::resource::Pools* Engine::pools() {
    return m_pools;
}

duk::resource::ResourceSet* Engine::resources() {
    return m_resources;
}

Director* Engine::director() {
    return m_director;
}

const duk::engine::Input* Engine::input() const {
    return m_input;
}

duk::tools::Timer* Engine::timer() {
    return m_timer;
}

duk::event::Dispatcher* Engine::dispatcher() {
    return m_dispatcher;
}

}// namespace duk::engine
