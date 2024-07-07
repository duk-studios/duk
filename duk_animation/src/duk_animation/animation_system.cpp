//
// Created by Ricardo on 26/04/2024.
//
#include <duk_animation/animation_system.h>
#include <duk_animation/animator.h>

#include <duk_engine/engine.h>

namespace duk::animation {

void AnimationSystem::update() {
    const auto deltaTime = global<duk::tools::Timer>()->delta_time();

    for (auto [animator]: all_components_of<Animator>()) {
        animator->controller->evaluate(animator, deltaTime);
    }
}

}// namespace duk::animation
