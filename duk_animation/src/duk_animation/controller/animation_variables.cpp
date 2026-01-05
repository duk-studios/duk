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

namespace duk::serial {

using namespace duk::animation;

void JsonPrimitiveValue<AnimationVariables>::write(rapidjson::Document& document, rapidjson::Value& json, const AnimationVariables& value) {
    // for historical reasons (and simpler json files), we serialize the internal map directly
    json_write_value(document, json, value.m_variables);
}

void JsonPrimitiveValue<AnimationVariables>::read(const rapidjson::Value& json, AnimationVariables& value) {
    json_read_value(json, value.m_variables);
}

}
