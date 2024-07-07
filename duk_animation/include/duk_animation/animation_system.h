//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_SYSTEM_H
#define DUK_ANIMATION_ANIMATION_SYSTEM_H

#include <duk_system/system.h>

namespace duk::animation {

class AnimationSystem : public duk::system::System {
public:
    void update() override;
};

}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_SYSTEM_H
