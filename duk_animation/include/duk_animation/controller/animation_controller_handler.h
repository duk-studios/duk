//
// Created by Ricardo on 30/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CONTROLLER_HANDLER_H
#define DUK_ANIMATION_ANIMATION_CONTROLLER_HANDLER_H

#include <duk_animation/controller/animation_controller.h>

#include <duk_resource/handler.h>

namespace duk::animation {

class AnimationControllerHandler : public duk::resource::TextHandlerT<AnimationController> {
public:
    AnimationControllerHandler();

    bool accepts(const std::string& extension) const override;

protected:
    std::shared_ptr<AnimationController> load_from_text(duk::tools::Globals* globals, const std::string_view& text) override;
};

}// namespace duk::animation

#endif//DUK_ANIMATION_ANIMATION_CONTROLLER_HANDLER_H
