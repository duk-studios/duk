//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_SET_H
#define DUK_ANIMATION_ANIMATION_SET_H

#include <duk_animation/clip/animation_clip.h>
#include <duk_animation/controller/animation_state.h>
#include <duk_animation/controller/animation_transition.h>

namespace duk::animation {

struct Animation {
    std::string name;
    AnimationClipResource clip;
    std::vector<AnimationTransition> transitions;
};

const Animation* find_animation(const std::vector<Animation>& animations, const std::string_view& name);

}// namespace duk::animation

namespace duk::type {
// clang-format off
template<>
struct Type<duk::animation::Animation> : Class<duk::animation::Animation,
    Member<"name", &duk::animation::Animation::name>,
    Member<"clip", &duk::animation::Animation::clip>,
    Member<"transitions", &duk::animation::Animation::transitions>> {
};

// clang-format on
}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::Animation& animation) {
    solver->solve(animation.clip);
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATION_SET_H
