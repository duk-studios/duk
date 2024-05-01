//
// Created by Ricardo on 28/04/2024.
//

#include <duk_animation/controller/animation_controller.h>

#include <duk_animation/animator.h>

namespace duk::animation {

void AnimationController::evaluate(const duk::objects::Component<Animator>& animator, float deltaTime) const {
    auto& state = animator->state;

    if (state.controller != this) {
        state = build_state();
    }

    const auto animation = state.animation;

    animation->clip->evaluate(animator.object(), state.time);

    state.time += deltaTime;

    for (auto& transition: animation->transitions) {
        if (transition.check(state)) {
            // sample at the end of the animation before transitioning
            animation->clip->evaluate(animator.object(), animation->clip->duration());
            state.animation = m_animations.at(transition.target());
            state.time = 0.0f;
        }
    }
}

AnimationSet& AnimationController::animations() {
    return m_animations;
}

const AnimationSet& AnimationController::animations() const {
    return m_animations;
}

AnimationState AnimationController::build_state() const {
    AnimationState state;
    state.controller = this;
    state.time = 0.0f;
    state.animation = m_animations.front();
    state.variables.assign(m_variables);
    return state;
}

}// namespace duk::animation
