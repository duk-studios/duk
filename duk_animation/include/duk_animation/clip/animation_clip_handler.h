//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H
#define DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H

#include <duk_animation/clip/animation_clip_pool.h>

#include <duk_resource/handler.h>

namespace duk::animation {

class AnimationClipHandler : public duk::resource::ResourceHandlerT<AnimationClipPool> {
public:
    AnimationClipHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<AnimationClip> load(AnimationClipPool* pool, const resource::Id& id, const std::filesystem::path& path) override;
};

}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_CLIP_HANDLER_H
