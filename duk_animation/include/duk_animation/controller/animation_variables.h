//
// Created by Ricardo on 30/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_VARIABLES_H
#define DUK_ANIMATION_ANIMATION_VARIABLES_H

#include <duk_serial/json.h>

#include <string>
#include <unordered_map>
#include <variant>

namespace duk::animation {

using Variable = std::variant<float, int, bool>;

class AnimationVariables {
public:
    const Variable& at(const std::string& name) const;

    void assign(const AnimationVariables& other);

    void set(const std::string& name, float value);

    void set(const std::string& name, int value);

    void set(const std::string& name, bool value);

    friend struct serial::JsonPrimitiveValue<AnimationVariables>;

private:
    std::unordered_map<std::string, Variable> m_variables;
};

}// namespace duk::animation

namespace duk::serial {

template<>
struct JsonPrimitiveValue<duk::animation::AnimationVariables> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::AnimationVariables& value);

    static void read(const rapidjson::Value& json, duk::animation::AnimationVariables& value);
};

}// namespace duk::serial

#endif//DUK_ANIMATION_ANIMATION_VARIABLES_H