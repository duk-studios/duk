//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_ANIMATION_CANVAS_PROPERTY_H
#define DUK_ANIMATION_CANVAS_PROPERTY_H

#include <duk_animation/clip/property.h>

namespace duk::animation {

struct CanvasAnchorValue {
    glm::vec2 anchor;
};

class CanvasAnchorProperty  {
public:
    using ValueType = CanvasAnchorValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasAnchorProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct CanvasPivotValue {
    glm::vec2 pivot;
};

class CanvasPivotProperty  {
public:
    using ValueType = CanvasPivotValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasPivotProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};


struct CanvasPositionValue {
    glm::vec2 position;
};

class CanvasPositionProperty  {
public:
    using ValueType = CanvasPositionValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasPositionProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

struct CanvasSizeValue {
    glm::vec2 size;
};

class CanvasSizeProperty  {
public:
    using ValueType = CanvasSizeValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<CanvasSizeProperty>* property, uint32_t sample);

    static ValueType interpolate(const ValueType& from, const ValueType& to, float progress);
};

}

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& value, duk::animation::CanvasAnchorValue& canvas) {
    from_json_member(value, "anchor", canvas.anchor);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& value, const duk::animation::CanvasAnchorValue& canvas) {
    to_json_member(document, value, "anchor", canvas.anchor);
}

template<>
inline void from_json(const rapidjson::Value& value, duk::animation::CanvasPivotValue& canvas) {
    from_json_member(value, "pivot", canvas.pivot);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& value, const duk::animation::CanvasPivotValue& canvas) {
    to_json_member(document, value, "pivot", canvas.pivot);
}

template<>
inline void from_json(const rapidjson::Value& value, duk::animation::CanvasPositionValue& canvas) {
    from_json_member(value, "position", canvas.position);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& value, const duk::animation::CanvasPositionValue& canvas) {
    to_json_member(document, value, "position", canvas.position);
}

template<>
inline void from_json(const rapidjson::Value& value, duk::animation::CanvasSizeValue& canvas) {
    from_json_member(value, "size", canvas.size);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& value, const duk::animation::CanvasSizeValue& canvas) {
    to_json_member(document, value, "size", canvas.size);
}

}

#endif //DUK_ANIMATION_CANVAS_PROPERTY_H
