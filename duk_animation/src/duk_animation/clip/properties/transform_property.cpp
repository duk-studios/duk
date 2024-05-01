//
// Created by Ricardo on 30/04/2024.
//

#include <duk_animation/clip/properties/transform_property.h>

#include <duk_renderer/components/transform.h>

namespace duk::animation {

void PositionProperty::evaluate(const duk::objects::Object& object, const PropertyT<PositionProperty>* property, uint32_t sample) {
    auto transform = object.component<duk::renderer::Transform>();
    if (!transform) {
        duk::log::warn("Transform not found for object with an animated position");
        return;
    }
    auto value = property->sample_interpolate(sample);
    transform->position = value.position;
}

PositionProperty::ValueType PositionProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.position = glm::mix(from.position, to.position, progress);
    return result;
}

void RotationProperty::evaluate(const duk::objects::Object& object, const PropertyT<RotationProperty>* property, uint32_t sample) {
    auto transform = object.component<duk::renderer::Transform>();
    if (!transform) {
        duk::log::warn("Transform not found for object with an animated rotation");
        return;
    }

    auto value = property->sample_interpolate(sample);
    transform->rotation = value.rotation;
}

RotationProperty::ValueType RotationProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.rotation = glm::mix(from.rotation, to.rotation, progress);
    return result;
}

void ScaleProperty::evaluate(const duk::objects::Object& object, const PropertyT<ScaleProperty>* property, uint32_t sample) {
    auto transform = object.component<duk::renderer::Transform>();
    if (!transform) {
        duk::log::warn("Transform not found for object with an animated scale");
        return;
    }

    auto value = property->sample_interpolate(sample);
    transform->scale = value.scale;
}

ScaleProperty::ValueType ScaleProperty::interpolate(const ValueType& from, const ValueType& to, float progress) {
    ValueType result;
    result.scale = glm::mix(from.scale, to.scale, progress);
    return result;
}

}
