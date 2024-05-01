//
// Created by Ricardo on 25/04/2024.
//

#include <duk_animation/clip/properties/sprite_property.h>

#include <duk_renderer/components/sprite_renderer.h>

namespace duk::animation {

void SpriteProperty::evaluate(const duk::objects::Object& object, const PropertyT<SpriteProperty>* property, uint32_t sample) {
    auto spriteRenderer = object.component<duk::renderer::SpriteRenderer>();
    if (!spriteRenderer) {
        duk::log::warn("SpriteRenderer not found for object with an animated SpriteProperty");
        return;
    }

    const auto& value = property->sample_at(sample);

    spriteRenderer->index = value.index;
    spriteRenderer->sprite = value.sprite;
}

}// namespace duk::animation
