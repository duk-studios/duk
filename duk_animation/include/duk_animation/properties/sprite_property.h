//
// Created by Ricardo on 25/04/2024.
//

#ifndef DUK_ANIMATION_SPRITE_PROPERTY_H
#define DUK_ANIMATION_SPRITE_PROPERTY_H

#include <duk_animation/property.h>

#include <duk_renderer/sprite/sprite.h>

namespace duk::animation {

struct SpriteValue {
    uint32_t index;
    duk::renderer::SpriteResource sprite;
};

class SpriteEvaluator {
public:
    using ValueType = SpriteValue;

    void evaluate(duk::objects::Object& object, const SpriteValue& sample) const;
};

}

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::SpriteValue& value) {
    from_json_member(json, "index", value.index);
    from_json_member(json, "sprite", value.sprite);
}

}

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::SpriteValue& value) {
    solver->solve(value.sprite);
}

}

#endif //DUK_ANIMATION_SPRITE_PROPERTY_H
