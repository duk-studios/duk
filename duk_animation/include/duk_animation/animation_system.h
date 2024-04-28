//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_SYSTEM_H
#define DUK_ANIMATION_ANIMATION_SYSTEM_H

#include <duk_animation/clip/animation_clip.h>

#include <duk_engine/systems.h>

namespace duk::animation {

struct Animator {
    AnimationClipResource animation;
    float time = 0.0f;
};

class AnimationSystem : public duk::engine::System {
public:
    AnimationSystem();

    void enter(duk::objects::Objects& objects, engine::Engine& engine) override;

    void update(duk::objects::Objects& objects, engine::Engine& engine) override;

    void exit(duk::objects::Objects& objects, engine::Engine& engine) override;
};

}

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::Animator& animator) {
    from_json_member(json, "animation", animator.animation);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::Animator& animator) {
    to_json_member(document, json, "animation", animator.animation);
}

}

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::Animator& animator) {
    solver->solve(animator.animation);
}

}

#endif //DUK_ANIMATION_ANIMATION_SYSTEM_H
