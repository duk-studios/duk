//
// Created by rov on 12/7/2023.
//

#ifndef DUK_IMPORT_SCENE_IMPORTER_H
#define DUK_IMPORT_SCENE_IMPORTER_H

#include <duk_import/resource_importer.h>
#include <duk_scene/scene.h>
#include <filesystem>

namespace duk::import {

class SceneImporter : public ResourceImporterT<duk::scene::Scene> {
public:
    SceneImporter();

    ~SceneImporter();

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

    duk::scene::Scene* find(duk::resource::Id id);

private:
    std::unordered_map<duk::resource::Id, std::unique_ptr<duk::scene::Scene>> m_scenes;
};

}

#endif //DUK_IMPORT_SCENE_IMPORTER_H
