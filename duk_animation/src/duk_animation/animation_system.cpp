//
// Created by Ricardo on 26/04/2024.
//
#include <duk_animation/animation_system.h>
#include <duk_animation/animator.h>

#include <duk_engine/engine.h>

namespace duk::animation {

AnimationSystem::AnimationSystem()
    : System(kMainThreadGroup) {
}

void AnimationSystem::enter(duk::objects::Objects& objects, engine::Engine& engine) {
}

void AnimationSystem::update(duk::objects::Objects& objects, engine::Engine& engine) {
    const auto deltaTime = engine.globals()->get<duk::tools::Timer>()->delta_time();

    for (auto object: objects.all_with<Animator>()) {
        auto animator = object.component<Animator>();
        animator->controller->evaluate(animator, deltaTime);
    }
}

void AnimationSystem::exit(duk::objects::Objects& objects, engine::Engine& engine) {
}

}// namespace duk::animation
