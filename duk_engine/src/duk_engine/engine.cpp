//
// Created by rov on 11/17/2023.
//

#include <duk_engine/engine.h>

#include <duk_log/log.h>

#include <duk_platform/platform.h>

#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/passes/present_pass.h>
#include <duk_renderer/builtins.h>

namespace duk::engine {

Engine::Engine(const EngineCreateInfo& engineCreateInfo)
    : m_run(false) {
    const auto& settings = engineCreateInfo.settings;
    const auto window = engineCreateInfo.window;

    m_globals.add_external<duk::platform::Platform>(engineCreateInfo.platform);

    if (window) {
        m_globals.add_external<duk::platform::Window>(engineCreateInfo.window);

        m_listener.listen(window->window_close_event, [window] {
            window->close();
        });

        m_listener.listen(window->window_destroy_event, [this] {
            m_run = false;
        });
    }

    // renderer
    {
        duk::renderer::RendererCreateInfo rendererCreateInfo = {};
        rendererCreateInfo.window = window;
        rendererCreateInfo.logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::VERBOSE));
        rendererCreateInfo.applicationName = settings.name.c_str();
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.apiValidationLayers = engineCreateInfo.rendererApiValidationLayers;

        m_globals.add<duk::renderer::Renderer>(rendererCreateInfo);
    }

    // audio
    {
        duk::audio::AudioCreateInfo audioEngineCreateInfo = {};
        audioEngineCreateInfo.channelCount = 2;
        audioEngineCreateInfo.frameRate = 48000;

        m_globals.add<duk::audio::Audio>(audioEngineCreateInfo);
    }

    // resources
    {
        duk::resource::ResourcesCreateInfo resourcesCreateInfo = {};
        resourcesCreateInfo.path = engineCreateInfo.workingDirectory / "resources";
        resourcesCreateInfo.loadMode = settings.loadMode;
        resourcesCreateInfo.globals = &m_globals;
        m_globals.add<duk::resource::Resources>(resourcesCreateInfo);
    }

    // builtin resources
    {
        const auto resources = m_globals.get<duk::resource::Resources>();
        const auto renderer = m_globals.get<duk::renderer::Renderer>();

        duk::renderer::BuiltinsCreateInfo builtinsCreateInfo = {};
        builtinsCreateInfo.pools = resources->pools();
        builtinsCreateInfo.rhi = renderer->rhi();
        builtinsCreateInfo.commandQueue = renderer->main_command_queue();
        builtinsCreateInfo.meshBufferPool = renderer->mesh_buffer_pool();

        m_globals.add<duk::renderer::Builtins>(builtinsCreateInfo);
    }

    // director
    {
        DirectorCreateInfo directorCreateInfo = {};
        directorCreateInfo.firstScene = settings.scene;

        m_globals.add<Director>(directorCreateInfo);
    }

    // input
    {
        InputCreateInfo inputCreateInfo = {};
        inputCreateInfo.window = window;
        m_globals.add<Input>(inputCreateInfo);
    }

    m_globals.add<duk::tools::Timer>();

    // render passes
    {
        const auto renderer = m_globals.get<duk::renderer::Renderer>();
        duk::renderer::ForwardPassCreateInfo forwardPassCreateInfo = {};
        forwardPassCreateInfo.rhi = renderer->rhi();
        forwardPassCreateInfo.commandQueue = renderer->main_command_queue();

        const auto forwardPass = renderer->add_pass<duk::renderer::ForwardPass>(forwardPassCreateInfo);

        if (window) {
            auto builtins = m_globals.get<duk::renderer::Builtins>();
            duk::renderer::PresentPassCreateInfo presentPassCreateInfo = {};
            presentPassCreateInfo.rhi = renderer->rhi();
            presentPassCreateInfo.commandQueue = renderer->main_command_queue();
            presentPassCreateInfo.shader = builtins->shader_pipelines()->fullscreen();
            presentPassCreateInfo.window = window;

            auto presentPass = renderer->add_pass<duk::renderer::PresentPass>(presentPassCreateInfo);

            forwardPass->out_color()->connect(presentPass->in_color());
        }
    }
}

Engine::~Engine() {
    m_globals.reset<Director>();
    m_globals.reset<duk::renderer::Builtins>();
    m_globals.reset<duk::resource::Resources>();
    m_globals.reset<duk::renderer::Renderer>();
}

void Engine::run() {
    m_run = true;

    const auto platform = m_globals.get<duk::platform::Platform>();
    const auto window = m_globals.get<duk::platform::Window>();
    const auto audio = m_globals.get<duk::audio::Audio>();
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
