//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_ANIMATION_CANVAS_PROPERTY_H
#define DUK_ANIMATION_CANVAS_PROPERTY_H

#include <duk_animation/clip/property.h>

#include <duk_math/glm.h>

namespace duk::animation {

struct CanvasAnchorValue {
    glm::vec2 anchor;
};

class CanvasAnchorProperty {
public:
    using ValueType = CanvasAnchorValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasAnchorProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct CanvasPivotValue {
    glm::vec2 pivot;
};

class CanvasPivotProperty {
public:
    using ValueType = CanvasPivotValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasPivotProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct CanvasPositionValue {
    glm::vec2 position;
};

class CanvasPositionProperty {
public:
    using ValueType = CanvasPositionValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasPositionProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct CanvasSizeValue {
    glm::vec2 size;
};

class CanvasSizeProperty {
public:
    using ValueType = CanvasSizeValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasSizeProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct CanvasScaleValue {
    glm::vec2 scale;
};

class CanvasScaleProperty {
public:
    using ValueType = CanvasScaleValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasScaleProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

}// namespace duk::animation

namespace duk::type {
// clang-format off
template<>
struct Type<duk::animation::CanvasAnchorValue> : Class<duk::animation::CanvasAnchorValue,
    Member<"anchor", &duk::animation::CanvasAnchorValue::anchor>> {
};

template<>
struct Type<duk::animation::CanvasPivotValue> : Class<duk::animation::CanvasPivotValue,
    Member<"pivot", &duk::animation::CanvasPivotValue::pivot>> {
};

template<>
struct Type<duk::animation::CanvasPositionValue> : Class<duk::animation::CanvasPositionValue,
    Member<"position", &duk::animation::CanvasPositionValue::position>> {
};

template<>
struct Type<duk::animation::CanvasSizeValue> : Class<duk::animation::CanvasSizeValue,
    Member<"size", &duk::animation::CanvasSizeValue::size>> {
};

template<>
struct Type<duk::animation::CanvasScaleValue> : Class<duk::animation::CanvasScaleValue,
    Member<"scale", &duk::animation::CanvasScaleValue::scale>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_ANIMATION_CANVAS_PROPERTY_H
