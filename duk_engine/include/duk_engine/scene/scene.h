/// 21/02/2024
/// scene.h

#ifndef DUK_ENGINE_SCENE_H
#define DUK_ENGINE_SCENE_H

#include <duk_engine/systems.h>
#include <duk_objects/objects.h>

namespace duk::engine {

class Scene {
public:
    duk::objects::Objects& objects();

    const duk::objects::Objects& objects() const;

    Systems& systems();

    const Systems& systems() const;

    void enter(Engine& engine);

    void update(Engine& engine, uint32_t activeSystemsGroup);

    void exit(Engine& engine);

private:
    duk::objects::Objects m_objects;
    Systems m_systems;
};

using SceneResource = duk::resource::ResourceT<Scene>;

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
