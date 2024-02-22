/// 03/06/2023
/// application.cpp

#include <duk_engine/systems/render_system.h>
#include <duk_import/scene/scene_importer.h>
#include <duk_sample/application.h>
#include <duk_sample/camera_system.h>

namespace duk::sample {

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) {
    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.applicationName = applicationCreateInfo.name;

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    duk::scene::register_system<CameraSystem>(*m_engine);

    auto importer = m_engine->importer();
    importer->load_resources("./");

    // this will be removed
    auto sceneId = importer->find_id("main");
    importer->load_resource(sceneId);

    auto sceneImporter = importer->get_importer_as<duk::import::SceneImporter>("scn");
    auto scene = sceneImporter->find(sceneId);

    m_engine->use_scene(scene);
}

Application::~Application() = default;

void Application::run() {
    m_engine->run();
}

}// namespace duk::sample
