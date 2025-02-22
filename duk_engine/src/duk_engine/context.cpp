//
// Created by rov on 11/17/2023.
//

#include <duk_engine/context.h>

#include <duk_log/log.h>

#include <duk_platform/platform.h>

#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/passes/present_pass.h>
#include <duk_renderer/builtins.h>

namespace duk::engine {

Context::Context(const ContextCreateInfo& contextCreateInfo) {
    duk::log::verb("Initializing context for {}", contextCreateInfo.settings.name);

    const auto& settings = contextCreateInfo.settings;
    const auto window = contextCreateInfo.window;

    m_platform = m_globals.add_external<duk::platform::Platform>(contextCreateInfo.platform);

    if (window) {
        m_window = m_globals.add_external<duk::platform::Window>(contextCreateInfo.window);
    }

    // renderer
    {
        duk::log::verb("Initializing context renderer");

        bool validationLayers = contextCreateInfo.rendererApiValidationLayers;
        duk::renderer::RendererCreateInfo rendererCreateInfo = {};
        rendererCreateInfo.window = window;
        rendererCreateInfo.logger = duk::log::add_logger("duk-rhi", validationLayers ? log::Level::INFO : log::Level::DEBUG);
        rendererCreateInfo.applicationName = settings.name.c_str();
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.apiValidationLayers = validationLayers;

        m_renderer = m_globals.add<duk::renderer::Renderer>(rendererCreateInfo);
    }

    // audio
    {
        duk::log::verb("Initializing context audio");
        duk::audio::AudioCreateInfo audioEngineCreateInfo = {};
        audioEngineCreateInfo.channelCount = 2;
        audioEngineCreateInfo.frameRate = 48000;

        m_audio = m_globals.add<duk::audio::Audio>(audioEngineCreateInfo);
        m_audio->start();
    }

    // resources
    {
        duk::log::verb("Initializing context resources");
        duk::resource::ResourcesCreateInfo resourcesCreateInfo = {};
        resourcesCreateInfo.path = contextCreateInfo.workingDirectory / "resources";
        resourcesCreateInfo.loadMode = settings.loadMode;
        resourcesCreateInfo.globals = &m_globals;
        m_resources = m_globals.add<duk::resource::Resources>(resourcesCreateInfo);
    }

    // builtin resources
    {
        duk::log::verb("Initializing context builtin resources");

        duk::renderer::BuiltinsCreateInfo builtinsCreateInfo = {};
        builtinsCreateInfo.pools = m_resources->pools();
        builtinsCreateInfo.rhi = m_renderer->rhi();
        builtinsCreateInfo.commandQueue = m_renderer->main_command_queue();
        builtinsCreateInfo.meshBufferPool = m_renderer->mesh_buffer_pool();

        m_builtins = m_globals.add<duk::renderer::Builtins>(builtinsCreateInfo);
    }

    // director
    {
        duk::log::verb("Initializing director");
        DirectorCreateInfo directorCreateInfo = {};
        directorCreateInfo.firstScene = settings.scene;

        m_director = m_globals.add<Director>(directorCreateInfo);
    }

    // input
    {
        duk::log::verb("Initializing input");
        InputCreateInfo inputCreateInfo = {};
        inputCreateInfo.window = window;
        m_input = m_globals.add<Input>(inputCreateInfo);
    }

    {
        m_timer = m_globals.add<duk::tools::Timer>();
        // assume 60fps for the first frame
        m_timer->add_duration(std::chrono::milliseconds(16));
    }

    // render passes
    {
        duk::log::verb("Initializing render passes");
        duk::renderer::ForwardPassCreateInfo forwardPassCreateInfo = {};
        forwardPassCreateInfo.rhi = m_renderer->rhi();
        forwardPassCreateInfo.commandQueue = m_renderer->main_command_queue();

        const auto forwardPass = m_renderer->add_pass<duk::renderer::ForwardPass>(forwardPassCreateInfo);

        if (window) {
            duk::renderer::PresentPassCreateInfo presentPassCreateInfo = {};
            presentPassCreateInfo.rhi = m_renderer->rhi();
            presentPassCreateInfo.commandQueue = m_renderer->main_command_queue();
            presentPassCreateInfo.shader = m_builtins->shader_pipelines()->fullscreen();
            presentPassCreateInfo.window = window;

            auto presentPass = m_renderer->add_pass<duk::renderer::PresentPass>(presentPassCreateInfo);

            forwardPass->out_color()->connect(presentPass->in_color());
        }
    }
}

Context::~Context() {
    duk::log::verb("Destroying context");
}

duk::platform::Platform* Context::platform() const {
    return m_platform;
}

duk::platform::Window* Context::window() const {
    return m_window;
}

duk::renderer::Renderer* Context::renderer() const {
    return m_renderer;
}

duk::resource::Resources* Context::resources() const {
    return m_resources;
}

duk::renderer::Builtins* Context::builtins() const {
    return m_builtins;
}

duk::audio::Audio* Context::audio() const {
    return m_audio;
}

Director* Context::director() const {
    return m_director;
}

Input* Context::input() const {
    return m_input;
}

duk::tools::Timer* Context::timer() const {
    return m_timer;
}

duk::tools::Globals* Context::globals() {
    return &m_globals;
}

}// namespace duk::engine
