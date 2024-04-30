//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_SYSTEM_H
#define DUK_ANIMATION_ANIMATION_SYSTEM_H

#include <duk_animation/controller/animation_set.h>

#include <duk_engine/systems.h>

namespace duk::animation {

class AnimationSystem : public duk::engine::System {
public:
    AnimationSystem();

    void enter(duk::objects::Objects& objects, engine::Engine& engine) override;

    void update(duk::objects::Objects& objects, engine::Engine& engine) override;

    void exit(duk::objects::Objects& objects, engine::Engine& engine) override;
};

}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_SYSTEM_H
