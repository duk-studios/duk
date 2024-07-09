/// 23/02/2024
/// director.cpp

#include <duk_engine/director.h>
#include <duk_engine/engine.h>

namespace duk::engine {

Director::Director(const DirectorCreateInfo& directorCreateInfo)
    : m_requestedSceneId(directorCreateInfo.firstScene)
    , m_disabledGroupsMask(0) {
}

Director::~Director() {
}

void Director::update(duk::tools::Globals& globals) {
    if (!m_requestedSceneAlias.empty()) {
        m_requestedSceneId = globals.get<duk::resource::Resources>()->find_id(m_requestedSceneAlias);
        if (m_requestedSceneId == duk::resource::kInvalidId) {
            throw std::runtime_error(fmt::format("failed to find id for scene with alias '{}'", m_requestedSceneAlias));
        }
        m_requestedSceneAlias.clear();
    }

    if (m_requestedSceneId != duk::resource::kInvalidId) {
        load_scene(globals, m_requestedSceneId);
        m_requestedSceneId = duk::resource::kInvalidId;
    }

    if (!m_scene) {
        duk::log::warn("No scene loaded, skipping frame...");
        return;
    }

    m_scene->update(m_disabledGroupsMask);

    globals.get<duk::renderer::Renderer>()->render(m_scene->objects());
}

void Director::request_scene(duk::resource::Id id) {
    m_requestedSceneId = id;
}

void Director::request_scene(const std::string& alias) {
    m_requestedSceneAlias = alias;
}

Scene* Director::scene() {
    return m_scene.get();
}

void Director::enable(uint32_t systemGroup) {
    m_disabledGroupsMask &= ~systemGroup;
}

void Director::disable(uint32_t systemGroup) {
    m_disabledGroupsMask |= systemGroup;
}

void Director::load_scene(duk::tools::Globals& globals, duk::resource::Id id) {
    const auto resources = globals.get<duk::resource::Resources>();
    const auto scene = resources->load<Scene>(id);

    if (!scene) {
        throw std::runtime_error(fmt::format("failed to load scene with id \"{}\"", m_requestedSceneId.value()));
    }
    if (m_scene) {
        m_scene->exit(m_disabledGroupsMask);
    }
    m_scene = scene;
    if (m_scene) {
        m_scene->attach(globals);
        m_scene->enter(m_disabledGroupsMask);
    }

    const auto renderer = globals.get<duk::renderer::Renderer>();

    resources->pools()->clear();
    renderer->clear_cache();
}

}// namespace duk::engine
