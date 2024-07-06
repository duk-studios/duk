//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H
#define DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H

#include <duk_system/system.h>

namespace duk::sample {

struct SpriteAnimator {};

class SpriteAnimatorSystem : public duk::system::System {
public:
    void enter(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void update(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void exit(duk::objects::Objects& objects, duk::tools::Globals& globals) override;
};

}// namespace duk::sample

#endif//DUK_SAMPLE_SPRITE_ANIMATOR_SYSTEM_H
