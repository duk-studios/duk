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

Application::Application() {
    duk::api::register_types();

    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.workingDirectory = ".";
    engineCreateInfo.settings = detail::load_settings();

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    m_engine->pools()->create_pool<duk::animation::AnimationClipPool>();
    m_engine->pools()->create_pool<duk::animation::AnimationControllerPool>();
}

Application::~Application() = default;

void Application::run() {
    duk_api_run_enter(*m_engine);
    m_engine->run();
    duk_api_run_exit(*m_engine);
}

}// namespace duk::runtime
