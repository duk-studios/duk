//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_STATE_H
#define DUK_ANIMATION_ANIMATION_STATE_H

#include <duk_animation/controller/animation_variables.h>

namespace duk::animation {

class AnimationController;
struct AnimationEntry;

struct AnimationState {
    AnimationVariables variables;
    const AnimationController* controller = nullptr;
    const AnimationEntry* animation = nullptr;
    float time = 0.0f;
};
}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_STATE_H
