/// 23/02/2024
/// director.cpp

#include <duk_engine/director.h>
#include <duk_import/scene/scene_importer.h>

namespace duk::engine {

Director::Director(const DirectorCreateInfo& directorCreateInfo)
    : m_renderer(directorCreateInfo.renderer)
    , m_environment(directorCreateInfo.environment)
    , m_importer(directorCreateInfo.importer)
    , m_scenePool(directorCreateInfo.scenePool)
    , m_requestedSceneId(directorCreateInfo.firstScene) {
}

Director::~Director() {
    if (m_scene) {
        m_scene->exit(m_environment);
    }
}

void Director::update() {
    if (m_requestedSceneId.valid()) {
        auto scene = load_scene(m_requestedSceneId);
        if (!scene) {
            throw std::runtime_error(fmt::format("failed to load scene with id \"{}\"", m_requestedSceneId.value()));
        }
        m_requestedSceneId = duk::resource::kInvalidId;
        replace_scene(scene);
    }
    if (!m_scene) {
        duk::log::warn("No scene loaded in Director, skipping frame...");
        return;
    }

    m_scene->update(m_environment);

    m_renderer->render(m_scene.get());
}

void Director::request_scene(duk::resource::Id id) {
    m_requestedSceneId = id;
}

duk::scene::Scene* Director::scene() {
    return m_scene.get();
}

duk::scene::SceneResource Director::load_scene(duk::resource::Id id) {
    m_importer->load_resource(id);
    return m_scenePool->find(id);
}

void Director::replace_scene(duk::scene::SceneResource scene) {
    if (m_scene) {
        m_scene->exit(m_environment);
    }
    m_scene = scene;
    if (m_scene) {
        m_scene->enter(m_environment);
    }
}

}// namespace duk::engine
