//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_POOL_H
#define DUK_ANIMATION_ANIMATION_POOL_H

#include <duk_animation/animation.h>

#include <duk_resource/pool.h>

namespace duk::animation {

class AnimationPool final : public duk::resource::PoolT<AnimationResource> {

};

}

#endif //DUK_ANIMATION_ANIMATION_POOL_H
