//
// Created by rov on 12/7/2023.
//

#ifndef DUK_ENGINE_SCENE_HANDLER_H
#define DUK_ENGINE_SCENE_HANDLER_H

#include <duk_engine/scene/scene_pool.h>

#include <duk_resource/handler.h>

#include <filesystem>

namespace duk::engine {

struct SceneHandlerCreateInfo {
    ScenePool* scenePool;
};

class SceneHandler : public duk::resource::ResourceHandler {
public:
    SceneHandler(const SceneHandlerCreateInfo& sceneHandlerCreateInfo);

    ~SceneHandler();

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    ScenePool* m_scenePool;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SCENE_HANDLER_H
