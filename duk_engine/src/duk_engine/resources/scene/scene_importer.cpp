//
// Created by rov on 12/7/2023.
//

#include <duk_engine/resources/scene/scene_importer.h>

#include <duk_tools/file.h>

namespace duk::engine {

SceneImporter::SceneImporter(const SceneImporterCreateInfo& sceneImporterCreateInfo)
    : m_scenePool(sceneImporterCreateInfo.scenePool) {
}

SceneImporter::~SceneImporter() = default;

const std::string& SceneImporter::tag() const {
    static const std::string scnTag("scn");
    return scnTag;
}

void SceneImporter::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    duk::serial::JsonReader reader(content.c_str());

    auto scene = std::make_shared<Scene>();

    reader.visit(*scene);

    m_scenePool->insert(id, scene);
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

}// namespace duk::engine
