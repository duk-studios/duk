//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CONTROLLER_H
#define DUK_ANIMATION_ANIMATION_CONTROLLER_H

#include <duk_animation/controller/animation.h>
#include <duk_animation/controller/animation_state.h>
#include <duk_animation/controller/animation_transition.h>

namespace duk::animation {

struct Animator;

class AnimationController {
public:
    void evaluate(const duk::objects::Component<Animator>& animator, float deltaTime) const;

    std::vector<Animation>& animations();

    const std::vector<Animation>& animations() const;

    friend struct duk::type::Type<AnimationController>;

private:
    AnimationState build_state() const;

private:
    std::vector<Animation> m_animations;
    AnimationVariables m_variables;
};

using AnimationControllerResource = duk::resource::Handle<AnimationController>;

}// namespace duk::animation

namespace duk::type {

// clang-format off
template<>
struct Type<duk::animation::AnimationController> : Class<duk::animation::AnimationController,
    Member<"animations", &duk::animation::AnimationController::m_animations>,
    Member<"variables", &duk::animation::AnimationController::m_variables>> {
};

// clang-format on

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::AnimationController& animationController) {
    for (auto& animation: animationController.animations()) {
        solver->solve(animation);
    }
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATION_CONTROLLER_H
