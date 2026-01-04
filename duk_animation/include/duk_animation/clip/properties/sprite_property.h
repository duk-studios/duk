//
// Created by Ricardo on 25/04/2024.
//

#ifndef DUK_ANIMATION_SPRITE_PROPERTY_H
#define DUK_ANIMATION_SPRITE_PROPERTY_H

#include <duk_animation/clip/property.h>

#include <duk_renderer/sprite/sprite.h>

namespace duk::animation {

struct SpriteValue {
    uint32_t index;
    duk::renderer::SpriteResource sprite;
};

class SpriteProperty {
public:
    using ValueType = SpriteValue;

    static void evaluate(const duk::objects::Object& object, const PropertyT<SpriteProperty>* property, uint32_t sample);
};

}// namespace duk::animation

namespace duk::type {

template<>
struct Type<duk::animation::SpriteValue> : Class<duk::animation::SpriteValue,
    Member<"index", &duk::animation::SpriteValue::index>,
    Member<"sprite", &duk::animation::SpriteValue::sprite>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::SpriteValue& value) {
    solver->solve(value.sprite);
}

}// namespace duk::resource

#endif//DUK_ANIMATION_SPRITE_PROPERTY_H
