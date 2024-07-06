/// 21/02/2024
/// scene.h

#ifndef DUK_ENGINE_SCENE_H
#define DUK_ENGINE_SCENE_H

#include <duk_system/system.h>
#include <duk_objects/objects.h>

namespace duk::engine {

class Scene {
public:
    Scene();

    duk::objects::Objects& objects();

    const duk::objects::Objects& objects() const;

    duk::system::Systems& systems();

    const duk::system::Systems& systems() const;

    void enter(duk::tools::Globals& globals, uint32_t disabledGroupsMask);

    void update(duk::tools::Globals& globals, uint32_t disabledGroupsMask);

    void exit(duk::tools::Globals& globals, uint32_t disabledGroupsMask);

private:
    duk::objects::ComponentEventDispatcher m_dispatcher;
    duk::objects::Objects m_objects;
    duk::system::Systems m_systems;
};

using SceneResource = duk::resource::Handle<Scene>;

}// namespace duk::engine

namespace duk::serial {

template<>
inline void from_json<duk::engine::Scene>(const rapidjson::Value& json, duk::engine::Scene& scene) {
    from_json_member(json, "objects", scene.objects());
    from_json_member(json, "systems", scene.systems());
}

template<>
inline void to_json<duk::engine::Scene>(rapidjson::Document& document, rapidjson::Value& json, const duk::engine::Scene& scene) {
    to_json_member(document, json, "objects", scene.objects());
    to_json_member(document, json, "systems", scene.systems());
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::engine::Scene& scene) {
    solver->solve(scene.objects());
}

}// namespace duk::resource

#endif// DUK_ENGINE_SCENE_H
