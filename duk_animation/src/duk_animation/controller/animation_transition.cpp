//
// Created by Ricardo on 28/04/2024.
//

#include <duk_animation/controller/animation_transition.h>
#include <duk_animation/controller/animation.h>

namespace duk::animation {

bool FinishedCondition::evaluate(const AnimationState& state) const {
    return state.time >= state.animation->clip->duration();
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

bool TriggerCondition::evaluate(const AnimationState& state) const {
    auto& trigger = state.variables.at(m_variableName);
    return std::holds_alternative<bool>(trigger) && std::get<bool>(trigger);
}

void TriggerCondition::execute(AnimationState& state) const {
    state.variables.set(m_variableName, false);
}

bool AnimationTransition::check(const AnimationState& state) const {
    return std::ranges::all_of(m_conditions, [&state](const auto& condition) {
        return evaluate(condition, state);
    });
}

void AnimationTransition::execute(const duk::objects::Object& object, AnimationState& state, const std::vector<Animation>& animations) const {
    // when a condition is met, it may need to alter some state (e.g. trigger)
    for (auto& condition: m_conditions) {
        if (const auto triggerCondition = std::get_if<TriggerCondition>(&condition)) {
            triggerCondition->execute(state);
        }
    }
    // sample at the end of the animation before transitioning
    state.animation->clip->evaluate(object, state.animation->clip->samples());
    state.animation = find_animation(animations, m_target);
    state.time = 0.0f;
}

bool evaluate(const Condition& condition, const AnimationState& state) {
    return std::visit(
            [&state](const auto& condition) {
                return condition.evaluate(state);
            },
            condition);
}

}// namespace duk::animation
