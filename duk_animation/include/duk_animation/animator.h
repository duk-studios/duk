//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATOR_H
#define DUK_ANIMATION_ANIMATOR_H

#include <duk_animation/controller/animation_controller.h>

namespace duk::animation {

struct Animator {
    AnimationControllerResource controller;
    AnimationState state;
};

}// namespace duk::animation

namespace duk::type {

template<>
struct Type<duk::animation::Animator> : Class<duk::animation::Animator,
    Member<"controller", &duk::animation::Animator::controller>,
    Member<"state", &duk::animation::Animator::state>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::Animator& animator) {
    solver->solve(animator.controller);
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATOR_H
