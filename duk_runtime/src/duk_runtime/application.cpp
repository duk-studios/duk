//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_api/register_types.h>
#include <duk_api/api.h>

#include <duk_animation/clip/animation_clip_pool.h>
#include <duk_animation/controller/animation_controller_handler.h>

#include <duk_serial/json/serializer.h>

namespace duk::runtime {

namespace detail {

static duk::resource::LoadMode load_mode() {
#ifdef DUK_PACK
    return duk::resource::LoadMode::PACKED;
#else
    return duk::resource::LoadMode::UNPACKED;
#endif
}

static duk::engine::Settings load_settings() {
    auto loadMode = load_mode();
    std::string settingsJson;
    switch (loadMode) {
        case resource::LoadMode::UNPACKED:
            settingsJson = duk::tools::load_text("settings.json");
            break;
        case resource::LoadMode::PACKED:
            settingsJson = duk::tools::load_compressed_text("settings.bin");
            break;
    }
    auto settings = duk::serial::read_json<duk::engine::Settings>(settingsJson);
    settings.loadMode = loadMode;
    return settings;
}

}// namespace detail

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) {
    duk::api::register_types();

    auto settings = detail::load_settings();
    auto platform = applicationCreateInfo.platform;

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = settings.name.c_str();
    windowCreateInfo.width = settings.resolution.x;
    windowCreateInfo.height = settings.resolution.y;

    m_window = platform->create_window(windowCreateInfo);

    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.workingDirectory = ".";
    engineCreateInfo.settings = detail::load_settings();
    engineCreateInfo.platform = platform;
    engineCreateInfo.window = m_window.get();

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    auto pools = m_engine->globals()->get<duk::resource::Pools>();
    pools->create_pool<duk::animation::AnimationClipPool>();
    pools->create_pool<duk::animation::AnimationControllerPool>();
}

Application::~Application() = default;

void Application::run() {
    duk_api_run_enter(*m_engine);
    m_engine->run();
    duk_api_run_exit(*m_engine);
}

}// namespace duk::runtime
