//
// Created by Ricardo on 30/04/2024.
//

#include <duk_animation/controller/animation_controller_handler.h>

#include <duk_serial/json/serializer.h>

namespace duk::animation {

AnimationControllerHandler::AnimationControllerHandler()
    : TextHandlerT("anc") {
}

bool AnimationControllerHandler::accepts(const std::string& extension) const {
    return extension == ".anc";
}

std::shared_ptr<AnimationController> AnimationControllerHandler::load_from_text(duk::tools::Globals* globals, const std::string_view& text) {
    auto controller = std::make_shared<AnimationController>();
    duk::serial::read_json(text, *controller);
    return controller;
}

}// namespace duk::animation
