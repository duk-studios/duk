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

    DUK_NO_DISCARD duk::objects::Objects& objects();

    DUK_NO_DISCARD const duk::objects::Objects& objects() const;

    DUK_NO_DISCARD duk::system::Systems& systems();

    DUK_NO_DISCARD const duk::system::Systems& systems() const;

    void attach(duk::tools::Globals& globals);

    void enter(uint32_t disabledGroupsMask);

    void update(uint32_t disabledGroupsMask);

    void exit(uint32_t disabledGroupsMask);

private:
    duk::objects::ComponentEventDispatcher m_componentDispatcher;
    duk::system::SystemEventDispatcher m_systemDispatcher;
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
    solver->solve(scene.systems());
    solver->solve(scene.objects());
}

}// namespace duk::resource

#endif// DUK_ENGINE_SCENE_H
