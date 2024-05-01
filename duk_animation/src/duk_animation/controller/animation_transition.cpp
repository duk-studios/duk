//
// Created by Ricardo on 28/04/2024.
//

#include <duk_animation/controller/animation_transition.h>
#include <duk_animation/controller/animation_set.h>

namespace duk::animation {

void Condition::execute(AnimationState& state) const {
}

ConditionType FinishedCondition::type() const {
    return ConditionType::FINISHED;
}

bool FinishedCondition::evaluate(const AnimationState& state) const {
    return state.time >= state.animation->clip->duration();
}

ConditionType ComparisonCondition::type() const {
    return ConditionType::COMPARISON;
}

bool ComparisonCondition::evaluate(const AnimationState& state) const {
    const auto& variable = state.variables.at(m_variableName);
    switch (m_operator) {
        case OperatorType::EQUAL:
            return variable == m_value;
        case OperatorType::NOT_EQUAL:
            return variable != m_value;
        case OperatorType::GREATER:
            return variable > m_value;
        case OperatorType::GREATER_EQUAL:
            return variable >= m_value;
        case OperatorType::LESS:
            return variable < m_value;
        case OperatorType::LESS_EQUAL:
            return variable <= m_value;
        default:
            duk::log::warn("Unknown operator");
            return false;
    }
}

ConditionType TriggerCondition::type() const {
    return ConditionType::TRIGGER;
}

bool TriggerCondition::evaluate(const AnimationState& state) const {
    auto& trigger = state.variables.at(m_variableName);
    return trigger.bool_value();
}

void TriggerCondition::execute(AnimationState& state) const {
    state.variables.set(m_variableName, false);
}

bool AnimationTransition::check(const AnimationState& state) const {
    return m_condition->evaluate(state);
}

void AnimationTransition::execute(const duk::objects::Object& object, AnimationState& state, const AnimationSet& animations) const {
    // when a condition is met, it may need to alter some state (e.g. trigger)
    m_condition->execute(state);
    // sample at the end of the animation before transitioning
    state.animation->clip->evaluate(object, state.animation->clip->samples());
    state.animation = animations.at(m_target);
    state.time = 0.0f;
}

std::string_view AnimationTransition::target() const {
    return m_target;
}

}// namespace duk::animation
