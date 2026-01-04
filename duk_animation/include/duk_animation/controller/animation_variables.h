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

    friend struct duk::type::Type<AnimationVariables>;

private:
    std::unordered_map<std::string, Variable> m_variables;
};

}// namespace duk::animation

namespace duk::type {

template<>
struct Type<duk::animation::AnimationVariables> : Class<duk::animation::AnimationVariables,
    Member<"variables", &duk::animation::AnimationVariables::m_variables>> {
};

}

#endif//DUK_ANIMATION_ANIMATION_VARIABLES_H