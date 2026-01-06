//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_TRANSITION_H
#define DUK_ANIMATION_ANIMATION_TRANSITION_H

#include <duk_animation/controller/animation_state.h>

#include <string>

#include <duk_objects/objects.h>

namespace duk::animation {

enum class OperatorType {
    UNDEFINED,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL
};

class FinishedCondition {
public:
    bool evaluate(const AnimationState& state) const;
};

class ComparisonCondition {
public:
    bool evaluate(const AnimationState& state) const;

    friend struct duk::type::Type<ComparisonCondition>;

private:
    std::string m_variableName;
    OperatorType m_operator;
    Variable m_value;
};

class TriggerCondition {
public:
    bool evaluate(const AnimationState& state) const;

    void execute(AnimationState& state) const;

    friend struct duk::type::Type<TriggerCondition>;

private:
    std::string m_variableName;
};

using Condition = std::variant<FinishedCondition, ComparisonCondition, TriggerCondition>;

bool evaluate(const Condition& condition, const AnimationState& state);

class AnimationTransition {
public:
    // returns true if the transition condition is met
    bool check(const AnimationState& state) const;

    void execute(const duk::objects::Object& object, AnimationState& state, const std::vector<Animation>& animations) const;

    friend struct duk::type::Type<AnimationTransition>;

private:
    std::string m_target;
    std::vector<Condition> m_conditions;
};

}// namespace duk::animation

namespace duk::type {

// clang-format off

template<>
struct Type<duk::animation::OperatorType> : Enum<duk::animation::OperatorType,
    Value<"undefined", duk::animation::OperatorType::UNDEFINED>,
    Value<"==", duk::animation::OperatorType::EQUAL>,
    Value<"!=", duk::animation::OperatorType::NOT_EQUAL>,
    Value<">", duk::animation::OperatorType::GREATER>,
    Value<">=", duk::animation::OperatorType::GREATER_EQUAL>,
    Value<"<", duk::animation::OperatorType::LESS>,
    Value<"<=", duk::animation::OperatorType::LESS_EQUAL>> {
};

template<>
struct Type<duk::animation::ComparisonCondition> : Class<duk::animation::ComparisonCondition,
    Member<"variable", &duk::animation::ComparisonCondition::m_variableName>,
    Member<"operator", &duk::animation::ComparisonCondition::m_operator>,
    Member<"value", &duk::animation::ComparisonCondition::m_value>> {
};

template<>
struct Type<duk::animation::TriggerCondition> : Class<duk::animation::TriggerCondition,
    Member<"variable", &duk::animation::TriggerCondition::m_variableName>> {
};

template<>
struct Type<duk::animation::AnimationTransition> : Class<duk::animation::AnimationTransition,
    Member<"target", &duk::animation::AnimationTransition::m_target>,
    Member<"conditions", &duk::animation::AnimationTransition::m_conditions>> {

};

// clang-format on

}// namespace duk::type

#endif//DUK_ANIMATION_ANIMATION_TRANSITION_H
