//
// Created by Ricardo on 26/04/2024.
//

#include <duk_animation/register_types.h>
#include <duk_animation/animator.h>
#include <duk_animation/animation_system.h>
#include <duk_animation/clip/animation_clip_handler.h>
#include <duk_animation/properties/sprite_property.h>
#include <duk_animation/controller/animation_controller_handler.h>

#include <duk_engine/register_types.h>
#include <duk_engine/systems.h>

namespace duk::animation {

void register_types() {
    duk::engine::register_types();
    register_property<SpriteEvaluator>();
    duk::resource::register_handler<AnimationClipHandler>();
    duk::resource::register_handler<AnimationControllerHandler>();
    duk::engine::register_system<AnimationSystem>();
    duk::objects::register_component<Animator>();
}

}// namespace duk::animation
