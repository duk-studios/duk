//
// Created by Ricardo on 30/04/2024.
//

#include <duk_animation/controller/animation_variables.h>
#include <duk_log/log.h>

namespace duk::animation {

const Variable& AnimationVariables::at(const std::string& name) const {
    return m_variables.at(name);
}

void AnimationVariables::assign(const AnimationVariables& other) {
    for (const auto& [name, variable]: other.m_variables) {
        // will not replace existing variables
        m_variables.emplace(name, variable);
    }
}

void AnimationVariables::set(const std::string& name, float value) {
    m_variables[name] = value;
}

void AnimationVariables::set(const std::string& name, int value) {
    m_variables[name] = value;
}

void AnimationVariables::set(const std::string& name, bool value) {
    m_variables[name] = value;
}

}// namespace duk::animation
