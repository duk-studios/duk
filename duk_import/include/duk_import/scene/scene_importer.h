//
// Created by rov on 12/7/2023.
//

#ifndef DUK_IMPORT_SCENE_IMPORTER_H
#define DUK_IMPORT_SCENE_IMPORTER_H

#include <duk_import/resource_importer.h>
#include <duk_scene/scene_pool.h>
#include <filesystem>

namespace duk::import {

struct SceneImporterCreateInfo {
    duk::scene::ScenePool* scenePool;
};

class SceneImporter : public ResourceImporterT<std::shared_ptr<duk::scene::Scene>> {
public:
    SceneImporter(const SceneImporterCreateInfo& sceneImporterCreateInfo);

    ~SceneImporter();

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    duk::scene::ScenePool* m_scenePool;
};

}// namespace duk::import

#endif//DUK_IMPORT_SCENE_IMPORTER_H
