//
// Created by Ricardo on 01/05/2024.
//

#include <duk_animation/clip/properties/canvas_property.h>
#include <duk_ui/components/canvas.h>

namespace duk::animation {

void CanvasPivotProperty::evaluate(const duk::objects::Object& object, const PropertyT<CanvasPivotProperty>* property, uint32_t sample) {
    auto canvasTransform = object.component<duk::ui::CanvasTransform>();
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

void CanvasScaleProperty::evaluate(const duk::objects::Object& object, const PropertyT<CanvasScaleProperty>* property, uint32_t sample) {
    auto canvasTransform = object.component<duk::ui::CanvasTransform>();
    if (!canvasTransform) {
        duk::log::warn("CanvasTransform not found for object with an animated scale");
        return;
    }
    auto value = property->sample_interpolate(sample);
    canvasTransform->scale = value.scale;
}

CanvasScaleProperty::ValueType CanvasScaleProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.scale = glm::mix(from.scale, to.scale, progress);
    return result;
}
}// namespace duk::animation
