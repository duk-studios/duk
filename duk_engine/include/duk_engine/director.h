/// 23/02/2024
/// director.h

#ifndef DUK_ENGINE_DIRECTOR_H
#define DUK_ENGINE_DIRECTOR_H

#include <duk_engine/scene/scene_pool.h>

#include <duk_renderer/renderer.h>
#include <duk_resource/set.h>

namespace duk::engine {

struct DirectorCreateInfo {
    duk::renderer::Renderer* renderer;
    duk::resource::ResourceSet* resources;
    duk::resource::Id firstScene;
};

class Director {
public:
    explicit Director(const DirectorCreateInfo& directorCreateInfo);

    ~Director();

    void update(Engine& engine);

    void request_scene(duk::resource::Id id);

    Scene* scene();

    void enable(uint32_t systemGroup);

    void disable(uint32_t systemGroup);

private:
    void load_scene(Engine& engine, duk::resource::Id id);

private:
    duk::renderer::Renderer* m_renderer;
    duk::resource::ResourceSet* m_resources;
    SceneResource m_scene;
    duk::resource::Id m_requestedSceneId;

private:
    uint32_t m_activeSystemGroup;
};

}// namespace duk::engine

#endif// DUK_ENGINE_DIRECTOR_H
