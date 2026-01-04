//
// Created by Ricardo on 30/04/2024.
//

#ifndef DUK_ANIMATION_TRANSFORM_PROPERTY_H
#define DUK_ANIMATION_TRANSFORM_PROPERTY_H

#include <duk_animation/clip/property.h>
#include <duk_math/glm.h>

namespace duk::animation {

struct PositionValue {
    glm::vec3 position;
};

class PositionProperty {
public:
    using ValueType = PositionValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<PositionProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct RotationValue {
    // needs to be a vec3, otherwise we cannot represent rotations bigger than 360 degrees
    glm::vec3 rotation;
};

class RotationProperty {
public:
    using ValueType = RotationValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<RotationProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct ScaleValue {
    glm::vec3 scale;
};

class ScaleProperty {
public:
    using ValueType = ScaleValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<ScaleProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

}// namespace duk::animation

namespace duk::type {

template<>
struct Type<duk::animation::PositionValue> : Class<duk::animation::PositionValue,
    Member<"position", &duk::animation::PositionValue::position>> {
};

template<>
struct Type<duk::animation::RotationValue> : Class<duk::animation::RotationValue,
    Member<"rotation", &duk::animation::RotationValue::rotation>> {
};

template<>
struct Type<duk::animation::ScaleValue> : Class<duk::animation::ScaleValue,
    Member<"scale", &duk::animation::ScaleValue::scale>> {
};

}// namespace duk::type

#endif//DUK_ANIMATION_TRANSFORM_PROPERTY_H
