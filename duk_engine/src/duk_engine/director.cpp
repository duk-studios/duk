/// 23/02/2024
/// director.cpp

#include <duk_engine/director.h>
#include <duk_import/scene/scene_importer.h>

namespace duk::engine {

Director::Director(const DirectorCreateInfo& directorCreateInfo)
    : m_renderer(directorCreateInfo.renderer)
    , m_importer(directorCreateInfo.importer)
    , m_scenePool(directorCreateInfo.scenePool)
    , m_requestedSceneId(directorCreateInfo.firstScene) {
}

Director::~Director() {
    if (m_scene) {
        m_scene->systems().exit();
    }
}

void Director::update() {
    if (m_requestedSceneId.valid()) {
        const auto id = m_requestedSceneId;
        m_requestedSceneId = duk::resource::kInvalidId;

        m_importer->load_resource(id);

        auto scene = m_scenePool->find(id);

        if (!scene) {
            throw std::runtime_error(fmt::format("could not load scene ({})", id.value()));
        }

        if (m_scene) {
            m_scene->systems().exit();
        }
        m_scene = scene;
        m_scene->systems().enter();
    }

    if (!m_scene) {
        duk::log::warn("No scene loaded in Director, skipping frame...");
        return;
    }

    m_scene->systems().update();

    m_renderer->render(m_scene.get());
}

void Director::load_scene(duk::resource::Id id) {
    m_requestedSceneId = id;
}

duk::scene::Scene* Director::scene() {
    return m_scene.get();
}

}// namespace duk::engine
