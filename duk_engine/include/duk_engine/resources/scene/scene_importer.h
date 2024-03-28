//
// Created by rov on 12/7/2023.
//

#ifndef DUK_ENGINE_SCENE_IMPORTER_H
#define DUK_ENGINE_SCENE_IMPORTER_H

#include <duk_engine/resources/scene/scene_pool.h>

#include <duk_resource/importer.h>

#include <filesystem>

namespace duk::engine {

struct SceneImporterCreateInfo {
    ScenePool* scenePool;
};

class SceneImporter : public duk::resource::ResourceImporter {
public:
    SceneImporter(const SceneImporterCreateInfo& sceneImporterCreateInfo);

    ~SceneImporter();

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    ScenePool* m_scenePool;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SCENE_IMPORTER_H
