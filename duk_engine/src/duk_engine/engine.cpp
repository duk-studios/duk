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
    : m_run(false) {
    const auto& settings = engineCreateInfo.settings;

    m_globals.add_external<duk::platform::Platform>(engineCreateInfo.platform);
    auto window = m_globals.add_external<duk::platform::Window>(engineCreateInfo.window);

    m_listener.listen(window->window_close_event, [window] {
        window->close();
    });

    m_listener.listen(window->window_destroy_event, [this] {
        m_run = false;
    });

    auto pools = m_globals.add<duk::resource::Pools>();

    {
        duk::renderer::RendererCreateInfo rendererCreateInfo = {};
        rendererCreateInfo.window = window;
        rendererCreateInfo.pools = pools;
        rendererCreateInfo.logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::VERBOSE));
        rendererCreateInfo.applicationName = settings.name.c_str();
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.apiValidationLayers = engineCreateInfo.rendererApiValidationLayers;

        auto renderer = m_globals.add<duk::renderer::Renderer>(rendererCreateInfo);
        duk::renderer::add_forward_passes(renderer, window);
    }

    {
        duk::audio::AudioEngineCreateInfo audioEngineCreateInfo = {};
        audioEngineCreateInfo.channelCount = 2;
        audioEngineCreateInfo.frameRate = 48000;

        m_globals.add<duk::audio::AudioEngine>(audioEngineCreateInfo);
    }

    {
        duk::resource::ResourceSetCreateInfo resourceSetCreateInfo = {};
        resourceSetCreateInfo.path = engineCreateInfo.workingDirectory / "resources";
        resourceSetCreateInfo.loadMode = settings.loadMode;
        resourceSetCreateInfo.pools = pools;
        m_globals.add<duk::resource::ResourceSet>(resourceSetCreateInfo);
    }

    /* init pools */
    {
        auto renderer = m_globals.get<duk::renderer::Renderer>();
        duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = renderer;
        pools->create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);

        auto shaderModulePool = pools->create_pool<duk::renderer::ShaderModulePool>();

        duk::renderer::ShaderPipelinePoolCreateInfo shaderPipelinePoolCreateInfo = {};
        shaderPipelinePoolCreateInfo.renderer = renderer;
        shaderPipelinePoolCreateInfo.shaderModulePool = shaderModulePool;
        pools->create_pool<duk::renderer::ShaderPipelinePool>(shaderPipelinePoolCreateInfo);

        duk::renderer::MaterialPoolCreateInfo materialPoolCreateInfo = {};
        materialPoolCreateInfo.renderer = renderer;
        pools->create_pool<duk::renderer::MaterialPool>(materialPoolCreateInfo);

        duk::renderer::MeshPoolCreateInfo meshPoolCreateInfo = {};
        meshPoolCreateInfo.meshBufferPool = renderer->mesh_buffer_pool();
        pools->create_pool<duk::renderer::MeshPool>(meshPoolCreateInfo);
        pools->create_pool<duk::renderer::SpritePool>();

        duk::audio::AudioClipPoolCreateInfo audioClipPoolCreateInfo = {};
        audioClipPoolCreateInfo.engine = m_globals.get<duk::audio::AudioEngine>();
        pools->create_pool<duk::audio::AudioClipPool>(audioClipPoolCreateInfo);

        pools->create_pool<duk::renderer::FontPool>();
        pools->create_pool<duk::objects::ObjectsPool>();
        pools->create_pool<ScenePool>();
    }

    // director
    {
        DirectorCreateInfo directorCreateInfo = {};
        directorCreateInfo.firstScene = settings.scene;

        m_globals.add<Director>(directorCreateInfo);
    }

    duk::engine::InputCreateInfo inputCreateInfo = {};
    inputCreateInfo.window = window;
    m_globals.add<Input>(inputCreateInfo);
    m_globals.add<duk::tools::Timer>();
    m_globals.add<duk::event::Dispatcher>();
}

Engine::~Engine() {
    m_globals.reset<Director>();
    m_globals.reset<duk::resource::ResourceSet>();
    m_globals.reset<resource::Pools>();
}

void Engine::run() {
    m_run = true;

    const auto platform = m_globals.get<duk::platform::Platform>();
    const auto window = m_globals.get<duk::platform::Window>();
    const auto audio = m_globals.get<duk::audio::AudioEngine>();
    const auto timer = m_globals.get<duk::tools::Timer>();
    const auto input = m_globals.get<Input>();
    const auto director = m_globals.get<Director>();

    window->show();

    audio->start();

    // assume 60fps for the first frame
    timer->add_duration(std::chrono::milliseconds(16));

    while (m_run) {
        timer->tick();

        input->refresh();

        platform->pool_events();

        while (window->valid() && window->minimized()) {
            platform->wait_events();
        }

        if (!window->valid()) {
            continue;
        }

        audio->update();

        director->update(m_globals);
    }
}

duk::tools::Globals* Engine::globals() {
    return &m_globals;
}

}// namespace duk::engine
