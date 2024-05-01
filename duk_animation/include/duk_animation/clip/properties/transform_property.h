//
// Created by Ricardo on 30/04/2024.
//

#ifndef DUK_ANIMATION_TRANSFORM_PROPERTY_H
#define DUK_ANIMATION_TRANSFORM_PROPERTY_H

#include <glm/gtc/random.hpp>

#include <duk_animation/clip/property.h>

namespace duk::animation {

struct PositionValue {
    glm::vec3 position;
};

class PositionProperty  {
public:
    using ValueType = PositionValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<PositionProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct RotationValue {
    // needs to be a vec3, otherwise we cannot represent rotations bigger than 360 degrees
    glm::vec3 rotation;
};

class RotationProperty  {
public:
    using ValueType = RotationValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<RotationProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct ScaleValue {
    glm::vec3 scale;
};

class ScaleProperty  {
public:
    using ValueType = ScaleValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<ScaleProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

}

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::PositionValue& value) {
    from_json_member(json, "position", value.position);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::PositionValue& value) {
    to_json_member(document, json, "position", value.position);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::RotationValue& value) {
    from_json_member(json, "rotation", value.rotation);
    value.rotation = glm::radians(value.rotation);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::RotationValue& value) {
    to_json_member(document, json, "rotation", glm::degrees(value.rotation));
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::ScaleValue& value) {
    from_json_member(json, "scale", value.scale);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::ScaleValue& value) {
    to_json_member(document, json, "scale", value.scale);
}

}

#endif //DUK_ANIMATION_TRANSFORM_PROPERTY_H
