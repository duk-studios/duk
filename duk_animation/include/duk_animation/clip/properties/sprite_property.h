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

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::SpriteValue& value) {
    from_json_member(json, "index", value.index);
    from_json_member(json, "sprite", value.sprite);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::SpriteValue& value) {
    to_json_member(document, json, "index", value.index);
    to_json_member(document, json, "sprite", value.sprite);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::SpriteValue& value) {
    solver->solve(value.sprite);
}

}// namespace duk::resource

#endif//DUK_ANIMATION_SPRITE_PROPERTY_H
