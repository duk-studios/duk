//
// Created by Ricardo on 30/04/2024.
//

#include <duk_animation/controller/animation_variables.h>
#include <duk_log/log.h>

namespace duk::animation {

Variable::Variable() {
    m_type = VariableType::UNDEFINED;
    m_value.intValue = 0;
}

Variable::Variable(float value) {
    m_type = VariableType::FLOAT;
    m_value.floatValue = value;
}

Variable::Variable(int value) {
    m_type = VariableType::INT;
    m_value.intValue = value;
}

Variable::Variable(bool value) {
    m_type = VariableType::BOOL;
    m_value.boolValue = value;
}

VariableType Variable::type() const {
    return m_type;
}

float Variable::float_value() const {
    DUK_ASSERT(m_type == VariableType::FLOAT);
    return m_value.floatValue;
}

int Variable::int_value() const {
    DUK_ASSERT(m_type == VariableType::INT);
    return m_value.intValue;
}

bool Variable::bool_value() const {
    DUK_ASSERT(m_type == VariableType::BOOL);
    return m_value.boolValue;
}

bool operator==(const Variable& lhs, const Variable& rhs) {
    DUK_ASSERT(lhs.type() == rhs.type());
    switch (lhs.type()) {
        case VariableType::FLOAT:
            return lhs.float_value() == rhs.float_value();
        case VariableType::INT:
            return lhs.int_value() == rhs.int_value();
        case VariableType::BOOL:
            return lhs.bool_value() == rhs.bool_value();
        case VariableType::UNDEFINED:
        default:
            return false;
    }
}

bool operator!=(const Variable& lhs, const Variable& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Variable& lhs, const Variable& rhs) {
    DUK_ASSERT(lhs.type() == rhs.type());
    switch (lhs.type()) {
        case VariableType::FLOAT:
            return lhs.float_value() < rhs.float_value();
        case VariableType::INT:
            return lhs.int_value() < rhs.int_value();
        case VariableType::BOOL:
            return lhs.bool_value() < rhs.bool_value();
        case VariableType::UNDEFINED:
        default:
            return false;
    }
}

bool operator>(const Variable& lhs, const Variable& rhs) {
    return rhs < lhs;
}

bool operator<=(const Variable& lhs, const Variable& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const Variable& lhs, const Variable& rhs) {
    return !(lhs < rhs);
}

const Variable& AnimationVariables::at(const std::string& name) const {
    return m_variables.at(name);
}

void AnimationVariables::assign(const AnimationVariables& other) {
    for (const auto& [name, variable] : other.m_variables) {
        // will not replace existing variables
        m_variables.emplace(name, variable);
    }
}

void AnimationVariables::set(const std::string& name, float value) {
    m_variables[name] = Variable(value);
}

void AnimationVariables::set(const std::string& name, int value) {
    m_variables[name] = Variable(value);
}

void AnimationVariables::set(const std::string& name, bool value) {
    m_variables[name] = Variable(value);
}

}// namespace duk::animation
