/// 23/02/2024
/// director.h

#ifndef DUK_ENGINE_DIRECTOR_H
#define DUK_ENGINE_DIRECTOR_H

#include <duk_import/importer.h>
#include <duk_renderer/renderer.h>
#include <duk_objects/scene_pool.h>

namespace duk::engine {

struct DirectorCreateInfo {
    duk::scene::Environment* environment;
    duk::renderer::Renderer* renderer;
    duk::import::Importer* importer;
    duk::scene::ScenePool* scenePool;
    duk::resource::Id firstScene;
};

class Director {
public:
    explicit Director(const DirectorCreateInfo& directorCreateInfo);

    ~Director();

    void update();

    void request_scene(duk::resource::Id id);

    duk::scene::Scene* scene();

private:
    duk::scene::SceneResource load_scene(duk::resource::Id id);

    void replace_scene(duk::scene::SceneResource scene);

private:
    duk::scene::Environment* m_environment;
    duk::renderer::Renderer* m_renderer;
    duk::import::Importer* m_importer;
    duk::scene::ScenePool* m_scenePool;
    duk::scene::SceneResource m_scene;
    duk::resource::Id m_requestedSceneId;
};

}// namespace duk::engine

#endif// DUK_ENGINE_DIRECTOR_H
