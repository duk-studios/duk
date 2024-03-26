/// 21/02/2024
/// scene.h

#ifndef DUK_OBJECTS_SCENE_H
#define DUK_OBJECTS_SCENE_H

#include <duk_objects/objects.h>
#include <duk_objects/systems.h>

namespace duk::scene {

class Scene {
public:
    Objects& objects();

    const Objects& objects() const;

    Systems& systems();

    const Systems& systems() const;

    void enter(Environment* environment);

    void update(Environment* environment);

    void exit(Environment* environment);

private:
    Objects m_objects;
    Systems m_systems;
};

using SceneResource = duk::resource::ResourceT<Scene>;

}// namespace duk::scene

namespace duk::serial {

template<typename JsonVisiter>
void visit_object(JsonVisiter* visiter, duk::scene::Scene& scene) {
    visiter->visit_member_array(scene.objects(), MemberDescription("objects"));
    visiter->visit_member_array(scene.systems(), MemberDescription("systems"));
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::scene::Scene& scene) {
    solver->solve(scene.objects());
}

}// namespace duk::resource

#endif// DUK_OBJECTS_SCENE_H
