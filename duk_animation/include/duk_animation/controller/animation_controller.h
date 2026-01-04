//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CONTROLLER_H
#define DUK_ANIMATION_ANIMATION_CONTROLLER_H

#include <duk_animation/controller/animation_set.h>
#include <duk_animation/controller/animation_state.h>
#include <duk_animation/controller/animation_transition.h>

namespace duk::animation {

struct Animator;

class AnimationController {
public:
    void evaluate(const duk::objects::Component<Animator>& animator, float deltaTime) const;

    AnimationSet& animations();

    const AnimationSet& animations() const;

    friend struct duk::type::Type<AnimationController>;

private:
    AnimationState build_state() const;

private:
    AnimationSet m_animations;
    AnimationVariables m_variables;
};

using AnimationControllerResource = duk::resource::Handle<AnimationController>;

}// namespace duk::animation

namespace duk::type {

template<>
struct Type<duk::animation::AnimationController> : Class<duk::animation::AnimationController,
    Member<"animations", &duk::animation::AnimationController::m_animations>,
    Member<"variables", &duk::animation::AnimationController::m_variables>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::AnimationController& animationController) {
    solver->solve(animationController.animations());
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATION_CONTROLLER_H
