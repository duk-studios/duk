//
// Created by Ricardo on 25/04/2024.
//

#include <duk_animation/properties/sprite_property.h>

#include <duk_renderer/components/sprite_renderer.h>

namespace duk::animation {

void SpriteEvaluator::evaluate(duk::objects::Object& object, const SpriteValue& sample) const {
    auto spriteRenderer = object.component<duk::renderer::SpriteRenderer>();
    if (!spriteRenderer) {
        duk::log::warn("SpriteRenderer not found for object with an animated SpriteProperty");
        return;
    }
    spriteRenderer->index = sample.index;
    spriteRenderer->sprite = sample.sprite;
}

}
