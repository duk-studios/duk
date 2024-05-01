//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H
#define DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H

#include <duk_engine/systems.h>

namespace duk::sample {

struct SpriteAnimator {

};

class SpriteAnimatorSystem : public duk::engine::System {
public:

    SpriteAnimatorSystem();

    void enter(duk::objects::Objects& objects, engine::Engine& engine) override;

    void update(duk::objects::Objects& objects, engine::Engine& engine) override;

    void exit(duk::objects::Objects& objects, engine::Engine& engine) override;
};

} // duk::sample

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::sample::SpriteAnimator& spriteAnimator) {

}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::sample::SpriteAnimator& spriteAnimator) {

}

}

#endif //DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H
