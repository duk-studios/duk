//
// Created by Ricardo on 01/05/2024.
//

#include <duk_sample/sprite_animator_system.h>

#include <duk_animation/animator.h>

#include <duk_engine/engine.h>

namespace duk::sample {
SpriteAnimatorSystem::SpriteAnimatorSystem()
    : System(kGameplayGroup) {
}

void SpriteAnimatorSystem::enter(duk::objects::Objects& objects, engine::Engine& engine) {
}

void SpriteAnimatorSystem::update(duk::objects::Objects& objects, engine::Engine& engine) {
    auto input = engine.input();

    auto rotating = input->key_down(platform::Keys::R);
    auto scaling = input->key_down(platform::Keys::T);

    for (auto object: objects.all_with<SpriteAnimator, duk::animation::Animator>()) {
        auto [spriteAnimator, animator] = object.components<SpriteAnimator, duk::animation::Animator>();
        if (rotating) {
            animator->state.variables.set("rotating", true);
        }
        if (scaling) {
            animator->state.variables.set("scaling", true);
        }
    }
}

void SpriteAnimatorSystem::exit(duk::objects::Objects& objects, engine::Engine& engine) {
}

}// namespace duk::sample