//
// Created by rov on 12/7/2023.
//

#include <duk_import/scene/scene_importer.h>
#include <duk_import/scene/scene_loader_json.h>

namespace duk::import {

SceneImporter::SceneImporter(const SceneImporterCreateInfo& sceneImporterCreateInfo)
    : m_scenePool(sceneImporterCreateInfo.scenePool) {
    add_loader<SceneLoaderJson>();
}

SceneImporter::~SceneImporter() = default;

const std::string& SceneImporter::tag() const {
    static const std::string scnTag("scn");
    return scnTag;
}

void SceneImporter::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    m_scenePool->insert(id, ResourceImporterT<std::shared_ptr<duk::scene::Scene>>::load(path));
}

void SceneImporter::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
    auto scene = m_scenePool->find(id);
    if (!scene) {
        throw std::logic_error("tried to solve dependencies of scene that was not loaded");
    }
    dependencySolver.solve(*scene);
}

void SceneImporter::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
    auto scene = m_scenePool->find(id);
    if (!scene) {
        throw std::logic_error("tried to solve references of scene that was not loaded");
    }
    referenceSolver.solve(*scene);
}

}// namespace duk::import
