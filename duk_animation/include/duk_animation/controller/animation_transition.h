//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_TRANSITION_H
#define DUK_ANIMATION_ANIMATION_TRANSITION_H

#include <duk_animation/controller/animation_state.h>

#include <string>

namespace duk::animation {

enum class ConditionType {
    UNDEFINED,
    FINISHED,
    COMPARISON
};

enum class OperatorType {
    UNDEFINED,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL
};

class Condition {
public:
    virtual ~Condition() = default;

    virtual bool evaluate(const AnimationState& state) const = 0;

    virtual ConditionType type() const = 0;
};

class FinishedCondition : public Condition {
public:
    bool evaluate(const AnimationState& state) const override;

    ConditionType type() const override;
};

class ComparisonCondition : public Condition {
public:
    bool evaluate(const AnimationState& state) const override;

    ConditionType type() const override;

    friend void duk::serial::from_json<ComparisonCondition>(const rapidjson::Value& json, ComparisonCondition& condition);

    friend void duk::serial::to_json<ComparisonCondition>(rapidjson::Document& document, rapidjson::Value& value, const ComparisonCondition& condition);

private:
    std::string m_variableName;
    OperatorType m_operator;
    Variable m_value;
};

class AnimationTransition {
public:
    // returns true if the transition condition is met
    bool check(const AnimationState& state) const;

    std::string_view target() const;

    friend void serial::from_json<AnimationTransition>(const rapidjson::Value& json, AnimationTransition& transition);

    friend void serial::to_json<AnimationTransition>(rapidjson::Document& document, rapidjson::Value& value, const AnimationTransition& transition);

private:
    std::string m_target;
    std::unique_ptr<Condition> m_condition;
};

}// namespace duk::animation

namespace duk::serial {

namespace detail {

static duk::animation::ConditionType parse_condition_type(const std::string_view& str) {
    if (str == "finished") {
        return duk::animation::ConditionType::FINISHED;
    }
    if (str == "comparison") {
        return duk::animation::ConditionType::COMPARISON;
    }
    return duk::animation::ConditionType::UNDEFINED;
}

static std::string to_string(const duk::animation::ConditionType& type) {
    switch (type) {
        case duk::animation::ConditionType::FINISHED:
            return "finished";
        case duk::animation::ConditionType::COMPARISON:
            return "comparison";
        case duk::animation::ConditionType::UNDEFINED:
        default:
            return "undefined";
    }
}

static duk::animation::OperatorType parse_operator_type(const std::string_view& str) {
    if (str == "==") {
        return duk::animation::OperatorType::EQUAL;
    }
    if (str == "!=") {
        return duk::animation::OperatorType::NOT_EQUAL;
    }
    if (str == ">") {
        return duk::animation::OperatorType::GREATER;
    }
    if (str == ">=") {
        return duk::animation::OperatorType::GREATER_EQUAL;
    }
    if (str == "<") {
        return duk::animation::OperatorType::LESS;
    }
    if (str == "<=") {
        return duk::animation::OperatorType::LESS_EQUAL;
    }
    return duk::animation::OperatorType::UNDEFINED;
}

static std::string to_string(const duk::animation::OperatorType& type) {
    switch (type) {
        case duk::animation::OperatorType::EQUAL:
            return "==";
        case duk::animation::OperatorType::NOT_EQUAL:
            return "!=";
        case duk::animation::OperatorType::GREATER:
            return ">";
        case duk::animation::OperatorType::GREATER_EQUAL:
            return ">=";
        case duk::animation::OperatorType::LESS:
            return "<";
        case duk::animation::OperatorType::LESS_EQUAL:
            return "<=";
        case duk::animation::OperatorType::UNDEFINED:
        default:
            return "undefined";
    }
}

}// namespace detail

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::ConditionType& type) {
    type = detail::parse_condition_type(json.GetString());
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::ConditionType& type) {
    to_json(document, json, detail::to_string(type));
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::OperatorType& type) {
    type = detail::parse_operator_type(json.GetString());
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::OperatorType& type) {
    to_json(document, json, detail::to_string(type));
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::ComparisonCondition& condition) {
    from_json_member(json, "variable", condition.m_variableName);
    from_json_member(json, "operator", condition.m_operator);
    from_json_member(json, "value", condition.m_value);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::ComparisonCondition& condition) {
    to_json_member(document, json, "variable", condition.m_variableName);
    to_json_member(document, json, "operator", condition.m_operator);
    to_json_member(document, json, "value", condition.m_value);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::FinishedCondition& condition) {
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::FinishedCondition& condition) {
}

template<>
inline void from_json(const rapidjson::Value& json, std::unique_ptr<duk::animation::Condition>& condition) {
    duk::animation::ConditionType type;
    from_json_member(json, "type", type);
    switch (type) {
        case animation::ConditionType::FINISHED: {
            auto finishedCondition = std::make_unique<duk::animation::FinishedCondition>();
            from_json(json, *finishedCondition);
            condition = std::move(finishedCondition);
            break;
        }
        case animation::ConditionType::COMPARISON: {
            auto comparisonCondition = std::make_unique<duk::animation::ComparisonCondition>();
            from_json(json, *comparisonCondition);
            condition = std::move(comparisonCondition);
            break;
        }
        default:
            throw std::runtime_error("Invalid condition type");
    }
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<duk::animation::Condition>& condition) {
    to_json_member(document, json, "type", condition->type());
    switch (condition->type()) {
        case animation::ConditionType::FINISHED:
            to_json(document, json, static_cast<const duk::animation::FinishedCondition&>(*condition));
            break;
        case animation::ConditionType::COMPARISON:
            to_json(document, json, static_cast<const duk::animation::ComparisonCondition&>(*condition));
            break;
        default:
            throw std::runtime_error("Invalid condition type");
    }
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::AnimationTransition& transition) {
    from_json_member(json, "target", transition.m_target);
    from_json_member(json, "condition", transition.m_condition);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::AnimationTransition& transition) {
    to_json_member(document, json, "target", transition.m_target);
    to_json_member(document, json, "condition", transition.m_condition);
}

}// namespace duk::serial

#endif//DUK_ANIMATION_ANIMATION_TRANSITION_H
