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

    friend void serial::from_json<AnimationController>(const rapidjson::Value& json, AnimationController& animationController);

    friend void serial::to_json<AnimationController>(rapidjson::Document& document, rapidjson::Value& json, const AnimationController& animationController);

private:
    AnimationState build_state() const;

private:
    AnimationSet m_animations;
    AnimationVariables m_variables;
};

using AnimationControllerResource = duk::resource::Handle<AnimationController>;

}// namespace duk::animation

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::AnimationController& animationController) {
    from_json_member(json, "animations", animationController.m_animations);
    from_json_member(json, "variables", animationController.m_variables);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::AnimationController& animationController) {
    to_json_member(document, json, "animations", animationController.m_animations);
    to_json_member(document, json, "variables", animationController.m_variables);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::AnimationController& animationController) {
    solver->solve(animationController.animations());
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATION_CONTROLLER_H
