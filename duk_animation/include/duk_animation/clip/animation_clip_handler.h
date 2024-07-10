//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H
#define DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H

#include <duk_animation/clip/animation_clip.h>

#include <duk_resource/handler.h>

namespace duk::animation {

class AnimationClipHandler : public duk::resource::TextHandlerT<AnimationClip> {
public:
    AnimationClipHandler();

    bool accepts(const std::string& extension) const override;

protected:
    std::shared_ptr<AnimationClip> load_from_text(duk::tools::Globals* globals, const std::string_view& text) override;
};

}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H
