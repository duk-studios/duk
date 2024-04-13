//
// Created by rov on 11/17/2023.
//

#include <duk_engine/engine.h>
#include <duk_engine/scene/scene_handler.h>

#include <duk_objects/objects_handler.h>

#include <duk_audio/clip/audio_clip_handler.h>

#include <duk_renderer/font/font_handler.h>
#include <duk_renderer/image/image_handler.h>
#include <duk_renderer/material/material_handler.h>
#include <duk_renderer/mesh/mesh_pool.h>
#include <duk_renderer/sprite/sprite_pool.h>

#include <duk_log/log.h>

#include <duk_platform/systems.h>

namespace duk::engine {

Engine::Engine(const EngineCreateInfo& engineCreateInfo)
    : m_run(false)
    , m_workingDirectory(engineCreateInfo.workingDirectory) {
    duk::platform::System::create();

    const auto settingsPath = m_workingDirectory / ".duk/settings.json";
    m_settings = load_settings(settingsPath.string());

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = m_settings.name.c_str();
    windowCreateInfo.width = m_settings.resolution.x;
    windowCreateInfo.height = m_settings.resolution.y;

    m_window = duk::platform::Window::create_window(windowCreateInfo);

    m_listener.listen(m_window->window_close_event, [this] {
        m_window->close();
    });

    m_listener.listen(m_window->window_destroy_event, [this]() {
        m_run = false;
    });

    {
        duk::renderer::RendererCreateInfo rendererCreateInfo = {};
        rendererCreateInfo.window = m_window.get();
        rendererCreateInfo.logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::DEBUG));
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.applicationName = m_settings.name.c_str();

        m_renderer = duk::renderer::make_forward_renderer(rendererCreateInfo);
    }

    {
        duk::audio::AudioDeviceCreateInfo audioDeviceCreateInfo = {};
        audioDeviceCreateInfo.backend = audio::Backend::MINIAUDIO;
        audioDeviceCreateInfo.channelCount = 2;
        audioDeviceCreateInfo.frameRate = 48000;

        m_audio = duk::audio::AudioDevice::create(audioDeviceCreateInfo);
    }

    {
        duk::resource::ResourceSetCreateInfo resourceSetCreateInfo = {};
        resourceSetCreateInfo.path = m_workingDirectory / "resources";
        resourceSetCreateInfo.pools = &m_pools;
        m_resources = std::make_unique<duk::resource::ResourceSet>(resourceSetCreateInfo);
    }

    /* init pools */
    // images
    {
        duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);
    }

    // font
    {
        m_pools.create_pool<duk::renderer::FontPool>();
    }

    // materials
    {
        duk::renderer::MaterialPoolCreateInfo materialPoolCreateInfo = {};
        materialPoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::MaterialPool>(materialPoolCreateInfo);
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

    // objects
    {
        m_pools.create_pool<duk::objects::ObjectsPool>();
    }

    // scenes
    {
        m_pools.create_pool<ScenePool>();
    }

    // audio
    {
        duk::audio::AudioClipPoolCreateInfo audioClipPoolCreateInfo = {};
        audioClipPoolCreateInfo.device = m_audio.get();
        m_pools.create_pool<duk::audio::AudioClipPool>(audioClipPoolCreateInfo);
    }

    // director
    {
        DirectorCreateInfo directorCreateInfo = {};
        directorCreateInfo.renderer = m_renderer.get();
        directorCreateInfo.resources = m_resources.get();
        directorCreateInfo.firstScene = m_settings.scene;

        m_director = std::make_unique<Director>(directorCreateInfo);
    }

    duk::engine::InputCreateInfo inputCreateInfo = {};
    inputCreateInfo.window = m_window.get();
    m_input = std::make_unique<duk::engine::Input>(inputCreateInfo);
}

Engine::~Engine() {
    m_director.reset();
    m_pools.clear();
    m_resources.reset();
}

void Engine::run() {
    m_run = true;

    m_window->show();

    // assume 60fps for the first frame
    m_timer.add_duration(std::chrono::milliseconds(16));

    m_audio->start();

    while (m_run) {
        m_timer.start();

        m_input->refresh();

        m_window->pool_events();

        while (m_window->minimized()) {
            m_window->wait_events();
        }

        m_audio->update();

        m_director->update(*this);

        m_timer.stop();
    }
}

duk::platform::Window* Engine::window() {
    return m_window.get();
}

duk::renderer::Renderer* Engine::renderer() {
    return m_renderer.get();
}

duk::audio::AudioDevice* Engine::audio() {
    return m_audio.get();
}

duk::resource::Pools* Engine::pools() {
    return &m_pools;
}

duk::resource::ResourceSet* Engine::resources() {
    return m_resources.get();
}

Director* Engine::director() {
    return m_director.get();
}

const duk::engine::Input* Engine::input() const {
    return m_input.get();
}

const duk::tools::Timer* Engine::timer() const {
    return &m_timer;
}

duk::event::Dispatcher* Engine::dispatcher() {
    return &m_dispatcher;
}

}// namespace duk::engine
