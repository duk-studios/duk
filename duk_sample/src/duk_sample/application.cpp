/// 03/06/2023
/// application.cpp

#include <duk_engine/systems/render_system.h>
#include <duk_import/scene/scene_importer.h>
#include <duk_sample/application.h>
#include <duk_sample/camera_system.h>

namespace duk::sample {

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) {
    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.settingsPath = applicationCreateInfo.engineSettingsPath;

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    duk::scene::register_system<CameraSystem>(*m_engine);
}

Application::~Application() = default;

void Application::run() {
    m_engine->run();
}

}// namespace duk::sample
