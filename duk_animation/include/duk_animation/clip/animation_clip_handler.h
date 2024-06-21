//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H
#define DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H

#include <duk_animation/clip/animation_clip_pool.h>

#include <duk_resource/handler.h>

namespace duk::animation {

class AnimationClipHandler : public duk::resource::TextResourceHandlerT<AnimationClipPool> {
public:
    AnimationClipHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<AnimationClip> load_from_text(AnimationClipPool* pool, const resource::Id& id, const std::string_view& text) override;
};

}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H
