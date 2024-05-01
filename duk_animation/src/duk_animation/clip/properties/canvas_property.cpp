//
// Created by Ricardo on 01/05/2024.
//

#include <duk_animation/clip/properties/canvas_property.h>
#include <duk_renderer/components/canvas.h>

namespace duk::animation {

void CanvasAnchorProperty::evaluate(const duk::objects::Object& object, const PropertyT<CanvasAnchorProperty>* property, uint32_t sample) {
    auto canvasTransform = object.component<duk::renderer::CanvasTransform>();
    if (!canvasTransform) {
        duk::log::warn("CanvasTransform not found for object with an animated anchor");
        return;
    }
    auto value = property->sample_interpolate(sample);
    canvasTransform->anchor = value.anchor;
}

CanvasAnchorProperty::ValueType CanvasAnchorProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.anchor = glm::mix(from.anchor, to.anchor, progress);
    return result;
}

void CanvasPivotProperty::evaluate(const duk::objects::Object& object, const PropertyT<CanvasPivotProperty>* property, uint32_t sample) {
    auto canvasTransform = object.component<duk::renderer::CanvasTransform>();
    if (!canvasTransform) {
        duk::log::warn("CanvasTransform not found for object with an animated pivot");
        return;
    }
    auto value = property->sample_interpolate(sample);
    canvasTransform->pivot = value.pivot;
}

CanvasPivotProperty::ValueType CanvasPivotProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.pivot = glm::mix(from.pivot, to.pivot, progress);
    return result;
}

void CanvasPositionProperty::evaluate(const duk::objects::Object& object, const PropertyT<CanvasPositionProperty>* property, uint32_t sample) {
    auto canvasTransform = object.component<duk::renderer::CanvasTransform>();
    if (!canvasTransform) {
        duk::log::warn("CanvasTransform not found for object with an animated position");
        return;
    }
    auto value = property->sample_interpolate(sample);
    canvasTransform->position = value.position;
}

CanvasPositionProperty::ValueType CanvasPositionProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.position = glm::mix(from.position, to.position, progress);
    return result;
}

void CanvasSizeProperty::evaluate(const duk::objects::Object& object, const PropertyT<CanvasSizeProperty>* property, uint32_t sample) {
    auto canvasTransform = object.component<duk::renderer::CanvasTransform>();
    if (!canvasTransform) {
        duk::log::warn("CanvasTransform not found for object with an animated size");
        return;
    }
    auto value = property->sample_interpolate(sample);
    canvasTransform->size = value.size;
}

CanvasSizeProperty::ValueType CanvasSizeProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.size = glm::mix(from.size, to.size, progress);
    return result;
}

}// namespace duk::animation
