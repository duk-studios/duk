/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_sample/camera_system.h>

namespace duk::sample {

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) {
    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.workingDirectory = applicationCreateInfo.engineWorkingDirectory;

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    duk::objects::register_component<CameraController>();
    duk::engine::register_system<CameraSystem>();
}

Application::~Application() = default;

void Application::run() {
    m_engine->run();
}

}// namespace duk::sample
