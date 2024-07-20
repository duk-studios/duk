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

namespace duk::serial {

template<>
inline void from_json<duk::sample::SpriteAnimator>(const rapidjson::Value& value, duk::sample::SpriteAnimator& animator) {
    from_json_member(value, "animator", animator.animator);
}

template<>
inline void to_json<duk::sample::SpriteAnimator>(rapidjson::Document& documnet, rapidjson::Value& value, const duk::sample::SpriteAnimator& animator) {
    to_json_member(documnet, value, "animator", animator.animator);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::sample::SpriteAnimator& spriteAnimator) {
    solver->solve(spriteAnimator.animator);
}

}// namespace duk::resource

#endif//DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H
