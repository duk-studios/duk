//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H
#define DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H

#include <duk_system/system.h>

#include <duk_animation/animator.h>

namespace duk::sample {

struct SpriteAnimator {
    duk::objects::Component<duk::animation::Animator> animator;
};

class SpriteAnimatorSystem : public duk::system::System {
public:
    void update() override;
};

}// namespace duk::sample

namespace duk::type {
// clang-format off
template<>
struct Type<duk::sample::SpriteAnimator> : Class<duk::sample::SpriteAnimator,
    Member<"animator", &duk::sample::SpriteAnimator::animator>> {
};

// clang-format on
}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::sample::SpriteAnimator& spriteAnimator) {
    solver->solve(spriteAnimator.animator);
}

}// namespace duk::resource

#endif//DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H
