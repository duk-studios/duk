/// 21/02/2024
/// scene.h

#ifndef DUK_ENGINE_SCENE_H
#define DUK_ENGINE_SCENE_H

#include <duk_engine/systems.h>
#include <duk_objects/objects.h>

namespace duk::engine {

class Scene {
public:
    duk::scene::Objects& objects();

    const duk::scene::Objects& objects() const;

    Systems& systems();

    const Systems& systems() const;

    void enter(Engine& engine);

    void update(Engine& engine);

    void exit(Engine& engine);

private:
    duk::scene::Objects m_objects;
    Systems m_systems;
};

using SceneResource = duk::resource::ResourceT<Scene>;

}// namespace duk::engine

namespace duk::serial {

template<typename JsonVisiter>
void visit_object(JsonVisiter* visiter, duk::engine::Scene& scene) {
    visiter->visit_member_array(scene.objects(), MemberDescription("objects"));
    visiter->visit_member_array(scene.systems(), MemberDescription("systems"));
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::engine::Scene& scene) {
    solver->solve(scene.objects());
}

}// namespace duk::resource

#endif// DUK_ENGINE_SCENE_H
