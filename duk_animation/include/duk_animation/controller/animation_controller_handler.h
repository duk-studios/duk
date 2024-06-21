//
// Created by Ricardo on 30/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CONTROLLER_HANDLER_H
#define DUK_ANIMATION_ANIMATION_CONTROLLER_HANDLER_H

#include <duk_animation/controller/animation_controller.h>

#include <duk_resource/handler.h>

namespace duk::animation {

class AnimationControllerPool : public duk::resource::PoolT<AnimationControllerResource> {};

class AnimationControllerHandler : public duk::resource::TextResourceHandlerT<AnimationControllerPool> {
public:
    AnimationControllerHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<AnimationController> load_from_text(AnimationControllerPool* pool, const resource::Id& id, const std::string_view& text) override;
};

}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_CONTROLLER_HANDLER_H
