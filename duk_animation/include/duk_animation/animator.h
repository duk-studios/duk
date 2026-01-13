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
// clang-format off
template<>
struct Type<duk::animation::Animator> : Class<duk::animation::Animator,
    Member<"controller", &duk::animation::Animator::controller>,
    Member<"state", &duk::animation::Animator::state>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_ANIMATION_ANIMATOR_H
