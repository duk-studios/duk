/// 21/02/2024
/// scene.h

#ifndef DUK_SCENE_SCENE_H
#define DUK_SCENE_SCENE_H

#include <duk_scene/objects.h>
#include <duk_scene/systems.h>

namespace duk::scene {

class Scene {
public:
    Objects& objects();

    const Objects& objects() const;

    Systems& systems();

    const Systems& systems() const;

private:
    Objects m_objects;
    Systems m_systems;
};

}// namespace duk::scene

namespace duk::json {

template<>
inline void from_json<duk::scene::Scene>(const rapidjson::Value& jsonObject, duk::scene::Scene& scene) {
    from_json_member(jsonObject, "objects", scene.objects());
    from_json_member(jsonObject, "systems", scene.systems());
}

}// namespace duk::json

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::scene::Scene& scene) {
    solver->solve(scene.objects());
}

}// namespace duk::resource

#endif// DUK_SCENE_SCENE_H
