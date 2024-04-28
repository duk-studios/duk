//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CLIP_POOL_H
#define DUK_ANIMATION_ANIMATION_CLIP_POOL_H

#include <duk_animation/clip/animation_clip.h>

#include <duk_resource/pool.h>

namespace duk::animation {

class AnimationClipPool final : public duk::resource::PoolT<AnimationClipResource> {

};

}

#endif //DUK_ANIMATION_ANIMATION_CLIP_POOL_H
