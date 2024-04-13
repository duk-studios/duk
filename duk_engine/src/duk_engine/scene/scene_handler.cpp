//
// Created by rov on 12/7/2023.
//

#include <duk_engine/scene/scene_handler.h>

#include <duk_tools/file.h>

namespace duk::engine {

SceneHandler::SceneHandler(const SceneHandlerCreateInfo& sceneHandlerCreateInfo)
    : m_scenePool(sceneHandlerCreateInfo.scenePool) {
}

SceneHandler::~SceneHandler() = default;

const std::string& SceneHandler::tag() const {
    static const std::string scnTag("scn");
    return scnTag;
}

void SceneHandler::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    duk::serial::JsonReader reader(content.c_str());

    auto scene = std::make_shared<Scene>();

    reader.visit(*scene);

    m_scenePool->insert(id, scene);
}

void SceneHandler::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
    auto scene = m_scenePool->find(id);
    if (!scene) {
        throw std::logic_error("tried to solve dependencies of scene that was not loaded");
    }
    dependencySolver.solve(*scene);
}

void SceneHandler::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
    auto scene = m_scenePool->find(id);
    if (!scene) {
        throw std::logic_error("tried to solve references of scene that was not loaded");
    }
    referenceSolver.solve(*scene);
}

}// namespace duk::engine
