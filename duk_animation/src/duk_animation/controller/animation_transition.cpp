//
// Created by Ricardo on 28/04/2024.
//

#include <duk_animation/controller/animation_transition.h>
#include <duk_animation/controller/animation_set.h>

namespace duk::animation {

bool FinishedCondition::evaluate(const AnimationState& state) const {
    return state.time >= state.animation->clip->duration();
}

ConditionType FinishedCondition::type() const {
    return ConditionType::FINISHED;
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

ConditionType ComparisonCondition::type() const {
    return ConditionType::COMPARISON;
}

bool AnimationTransition::check(const AnimationState& state) const {
    return m_condition->evaluate(state);
}

std::string_view AnimationTransition::target() const {
    return m_target;
}

}// namespace duk::animation
