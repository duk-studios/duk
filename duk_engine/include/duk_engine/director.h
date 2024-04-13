/// 23/02/2024
/// director.h

#ifndef DUK_ENGINE_DIRECTOR_H
#define DUK_ENGINE_DIRECTOR_H

#include <duk_engine/scene/scene_pool.h>

#include <duk_renderer/renderer.h>
#include <duk_resource/handler.h>

namespace duk::engine {

struct DirectorCreateInfo {
    duk::renderer::Renderer* renderer;
    duk::resource::Handler* handler;
    ScenePool* scenePool;
    duk::resource::Id firstScene;
};

class Director {
public:
    explicit Director(const DirectorCreateInfo& directorCreateInfo);

    ~Director();

    void update(Engine& engine);

    void request_scene(duk::resource::Id id);

    Scene* scene();

private:
    void load_scene(Engine& engine, duk::resource::Id id);

private:
    duk::renderer::Renderer* m_renderer;
    duk::resource::Handler* m_handler;
    ScenePool* m_scenePool;
    SceneResource m_scene;
    duk::resource::Id m_requestedSceneId;
};

}// namespace duk::engine

#endif// DUK_ENGINE_DIRECTOR_H
