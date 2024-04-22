/// 23/02/2024
/// director.cpp

#include <duk_engine/director.h>
#include <duk_engine/engine.h>

namespace duk::engine {

Director::Director(const DirectorCreateInfo& directorCreateInfo)
    : m_renderer(directorCreateInfo.renderer)
    , m_resources(directorCreateInfo.resources)
    , m_requestedSceneId(directorCreateInfo.firstScene)
    , m_activeSystemGroup(0) {
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

    m_scene->update(engine, m_activeSystemGroup);

    m_renderer->render(m_scene->objects());
}

void Director::request_scene(duk::resource::Id id) {
    m_requestedSceneId = id;
}

Scene* Director::scene() {
    return m_scene.get();
}

void Director::enable(uint32_t systemGroup) {
    m_activeSystemGroup |= systemGroup;
}

void Director::disable(uint32_t systemGroup) {
    m_activeSystemGroup &= ~systemGroup;
}

void Director::load_scene(Engine& engine, duk::resource::Id id) {
    m_resources->load(id);

    auto scene = m_resources->pools()->get<ScenePool>()->find(id);
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

    m_resources->pools()->clear();
}

}// namespace duk::engine
