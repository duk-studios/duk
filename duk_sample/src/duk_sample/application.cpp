/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_sample/camera_system.h>
#include <duk_engine/systems/render_system.h>

namespace duk::sample {

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) {

    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.applicationName = applicationCreateInfo.name;

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    auto importer = m_engine->importer();
    importer->load_resources("resources.json");

    m_scene = importer->load_scene("main");

    m_engine->use_scene(m_scene.get());

    auto systems = m_engine->systems();
    systems->add_system<CameraSystem>(*m_engine, "CameraSystem");
    systems->add_system<duk::engine::RenderSystem>(*m_engine, "RenderSystem");
}

Application::~Application() {

}

void Application::run() {
    m_engine->run();
}

}

