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

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::Animator& animator) {
    from_json_member(json, "controller", animator.controller);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::Animator& animator) {
    to_json_member(document, json, "controller", animator.controller);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::Animator& animator) {
    solver->solve(animator.controller);
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATOR_H
