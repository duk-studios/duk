//
// Created by Ricardo on 30/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_VARIABLES_H
#define DUK_ANIMATION_ANIMATION_VARIABLES_H

#include <duk_serial/json/types.h>

#include <string>
#include <unordered_map>

namespace duk::animation {

enum class VariableType {
    UNDEFINED,
    FLOAT,
    INT,
    BOOL,
};

class Variable {
public:
    union Value {
        float floatValue;
        int intValue;
        bool boolValue;
    };

    Variable();

    Variable(float value);

    Variable(int value);

    Variable(bool value);

    VariableType type() const;

    float float_value() const;

    int int_value() const;

    bool bool_value() const;

private:
    VariableType m_type;
    Value m_value;
};

bool operator==(const Variable& lhs, const Variable& rhs);

bool operator!=(const Variable& lhs, const Variable& rhs);

bool operator<(const Variable& lhs, const Variable& rhs);

bool operator>(const Variable& lhs, const Variable& rhs);

bool operator<=(const Variable& lhs, const Variable& rhs);

bool operator>=(const Variable& lhs, const Variable& rhs);

class AnimationVariables {
public:
    const Variable& at(const std::string& name) const;

    void assign(const AnimationVariables& other);

    void set(const std::string& name, float value);

    void set(const std::string& name, int value);

    void set(const std::string& name, bool value);

    friend void serial::from_json<AnimationVariables>(const rapidjson::Value& json, AnimationVariables& state);

    friend void serial::to_json<AnimationVariables>(rapidjson::Document& document, rapidjson::Value& json, const AnimationVariables& state);

private:
    std::unordered_map<std::string, Variable> m_variables;
};

}// namespace duk::animation

namespace duk::serial {

namespace detail {

static duk::animation::VariableType parse_variable_type(const std::string_view& type) {
    if (type == "float") {
        return duk::animation::VariableType::FLOAT;
    }
    if (type == "int") {
        return duk::animation::VariableType::INT;
    }
    if (type == "bool") {
        return duk::animation::VariableType::BOOL;
    }
    throw std::runtime_error(fmt::format("Invalid variable type '{}'", type));
}

static std::string to_string(duk::animation::VariableType type) {
    switch (type) {
        case duk::animation::VariableType::FLOAT:
            return "float";
        case duk::animation::VariableType::INT:
            return "int";
        case duk::animation::VariableType::BOOL:
            return "bool";
        default:
            throw std::runtime_error(fmt::format("Invalid variable type '{}'", static_cast<int>(type)));
    }
}

}// namespace detail

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::VariableType& variableType) {
    variableType = detail::parse_variable_type(json.GetString());
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::VariableType& variableType) {
    to_json(document, json, detail::to_string(variableType));
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::Variable& variable) {
    duk::animation::VariableType type;
    from_json_member(json, "type", type);
    switch (type) {
        case animation::VariableType::FLOAT: {
            float value;
            from_json_member(json, "value", value);
            variable = duk::animation::Variable(value);
            break;
        }
        case animation::VariableType::INT: {
            int value;
            from_json_member(json, "value", value);
            variable = duk::animation::Variable(value);
            break;
        }
        case animation::VariableType::BOOL: {
            bool value;
            from_json_member(json, "value", value);
            variable = duk::animation::Variable(value);
            break;
        }
        default:
            throw std::runtime_error(fmt::format("Invalid variable type '{}'", static_cast<int>(type)));
    }
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::Variable& variable) {
    to_json_member(document, json, "type", variable.type());
    switch (variable.type()) {
        case animation::VariableType::FLOAT:
            to_json_member(document, json, "value", variable.float_value());
            break;
        case animation::VariableType::INT:
            to_json_member(document, json, "value", variable.int_value());
            break;
        case animation::VariableType::BOOL:
            to_json_member(document, json, "value", variable.bool_value());
            break;
        default:
            break;
    }
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::AnimationVariables& variables) {
    auto jsonArray = json.GetArray();
    variables.m_variables.clear();
    for (const auto& jsonElement: jsonArray) {
        std::string name;
        from_json_member(jsonElement, "name", name);

        duk::animation::Variable variable;
        from_json(jsonElement, variable);

        variables.m_variables.emplace(name, variable);
    }
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::AnimationVariables& state) {
    auto jsonArray = json.SetArray().GetArray();
    for (const auto& [name, variable]: state.m_variables) {
        rapidjson::Value jsonElement;
        to_json_member(document, jsonElement, "name", name);
        to_json(document, jsonElement, variable);
        jsonArray.PushBack(jsonElement, document.GetAllocator());
    }
}

}// namespace duk::serial

#endif//DUK_ANIMATION_ANIMATION_VARIABLES_H
