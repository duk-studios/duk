//
// Created by Ricardo on 01/05/2024.
//

#include <duk_sample/sprite_animator_system.h>

#include <duk_animation/animator.h>

#include <duk_engine/engine.h>

namespace duk::sample {

void SpriteAnimatorSystem::update() {
    auto input = global<duk::engine::Input>();

    if (input->key_down(platform::Keys::R)) {
        auto director = global<duk::engine::Director>();
        director->request_scene("directions");
    }

    auto rotating = input->key_down(platform::Keys::G);
    auto scaling = input->key_down(platform::Keys::H);

    for (auto [spriteAnimator, animator]: all_components_of<SpriteAnimator, duk::animation::Animator>()) {
        if (rotating) {
            animator->state.variables.set("rotating", true);
        }
        if (scaling) {
            animator->state.variables.set("scaling", true);
        }
    }
}

}// namespace duk::sample