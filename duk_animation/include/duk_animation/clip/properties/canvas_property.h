//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_ANIMATION_CANVAS_PROPERTY_H
#define DUK_ANIMATION_CANVAS_PROPERTY_H

#include <duk_animation/clip/property.h>

namespace duk::animation {

struct CanvasPivotValue {
    glm::vec2 pivot;
};

class CanvasPivotProperty {
public:
    using ValueType = CanvasPivotValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasPivotProperty>* property, uint32_t sample);

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

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& value, duk::animation::CanvasPivotValue& canvas) {
    from_json_member(value, "pivot", canvas.pivot);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& value, const duk::animation::CanvasPivotValue& canvas) {
    to_json_member(document, value, "pivot", canvas.pivot);
}

template<>
inline void from_json(const rapidjson::Value& value, duk::animation::CanvasScaleValue& canvas) {
    from_json_member(value, "scale", canvas.scale);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& value, const duk::animation::CanvasScaleValue& canvas) {
    to_json_member(document, value, "scale", canvas.scale);
}

}// namespace duk::serial

#endif//DUK_ANIMATION_CANVAS_PROPERTY_H
