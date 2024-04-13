/// 23/02/2024
/// director.cpp

#include <duk_engine/director.h>
#include <duk_engine/engine.h>

namespace duk::engine {

Director::Director(const DirectorCreateInfo& directorCreateInfo)
    : m_renderer(directorCreateInfo.renderer)
    , m_handler(directorCreateInfo.handler)
    , m_scenePool(directorCreateInfo.scenePool)
    , m_requestedSceneId(directorCreateInfo.firstScene) {
}

Director::~Director() {
}

void Director::update(Engine& engine) {
    if (m_requestedSceneId != duk::resource::kInvalidId) {
        load_scene(engine, m_requestedSceneId);
        m_requestedSceneId = duk::resource::kInvalidId;
    }

    if (!m_scene) {
        duk::log::warn("No scene loaded, skipping frame...");
        return;
    }

    m_scene->update(engine);

    m_renderer->render(m_scene->objects());
}

void Director::request_scene(duk::resource::Id id) {
    m_requestedSceneId = id;
}

Scene* Director::scene() {
    return m_scene.get();
}

void Director::load_scene(Engine& engine, duk::resource::Id id) {
    m_handler->load_resource(id);

    auto scene = m_scenePool->find(id);
    if (!scene) {
        throw std::runtime_error(fmt::format("failed to load scene with id \"{}\"", m_requestedSceneId.value()));
    }
    if (m_scene) {
        m_scene->exit(engine);
    }
    m_scene = scene;
    if (m_scene) {
        m_scene->enter(engine);
    }

    engine.pools()->clear();
}

}// namespace duk::engine
