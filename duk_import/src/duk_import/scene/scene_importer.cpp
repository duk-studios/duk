//
// Created by rov on 12/7/2023.
//

#include <duk_import/scene/scene_importer.h>
#include <duk_import/scene/scene_loader_json.h>

namespace duk::import {

SceneImporter::SceneImporter() {
    add_loader<SceneLoaderJson>();
}

SceneImporter::~SceneImporter() {
    m_scenes.clear();
}

const std::string& SceneImporter::tag() const {
    static const std::string scnTag("scn");
    return scnTag;
}

void SceneImporter::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    m_scenes.clear();
    auto scene = ResourceImporterT<duk::scene::Scene>::load(path);
    m_scenes.emplace(id, std::move(scene));
}

void SceneImporter::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
    auto scene = find(id);
    if (!scene) {
        throw std::logic_error("tried to solve dependencies of scene that was not loaded");
    }
    dependencySolver.solve(*scene);
}

void SceneImporter::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
    auto scene = find(id);
    if (!scene) {
        throw std::logic_error("tried to solve references of scene that was not loaded");
    }
    referenceSolver.solve(*scene);
}

duk::scene::Scene* SceneImporter::find(duk::resource::Id id) {
    auto it = m_scenes.find(id);
    if (it == m_scenes.end()) {
        return nullptr;
    }
    return it->second.get();
}

}// namespace duk::import
