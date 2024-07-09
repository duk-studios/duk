/// 23/02/2024
/// director.h

#ifndef DUK_ENGINE_DIRECTOR_H
#define DUK_ENGINE_DIRECTOR_H

#include <duk_engine/scene/scene.h>

#include <duk_renderer/renderer.h>
#include <duk_resource/resources.h>

namespace duk::engine {

struct DirectorCreateInfo {
    duk::resource::Id firstScene;
};

class Director {
public:
    explicit Director(const DirectorCreateInfo& directorCreateInfo);

    ~Director();

    void update(duk::tools::Globals& globals);

    void request_scene(duk::resource::Id id);

    void request_scene(const std::string& alias);

    Scene* scene();

    void enable(uint32_t systemGroup);

    void disable(uint32_t systemGroup);

private:
    void load_scene(duk::tools::Globals& globals, duk::resource::Id id);

private:
    SceneResource m_scene;
    duk::resource::Id m_requestedSceneId;
    std::string m_requestedSceneAlias;
    uint32_t m_disabledGroupsMask;
};

}// namespace duk::engine

#endif// DUK_ENGINE_DIRECTOR_H
