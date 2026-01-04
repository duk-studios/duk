//
// Created by Ricardo on 26/04/2024.
//

#include <duk_animation/clip/animation_clip_handler.h>

namespace duk::animation {

AnimationClipHandler::AnimationClipHandler()
    : TextHandlerT("ani") {
}

bool AnimationClipHandler::accepts(const std::string& extension) const {
    return extension == ".ani";
}

std::shared_ptr<AnimationClip> AnimationClipHandler::load_from_text(duk::tools::Globals* globals, const std::string_view& text) {
    const auto animation = std::make_shared<AnimationClip>();

    duk::serial::json_read(text, *animation);

    return animation;
}

}// namespace duk::animation
