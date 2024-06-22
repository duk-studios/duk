//
// Created by Ricardo on 30/04/2024.
//

#include <duk_animation/controller/animation_controller_handler.h>

#include <duk_serial/json/serializer.h>

namespace duk::animation {

AnimationControllerHandler::AnimationControllerHandler()
    : TextResourceHandlerT("anc") {
}

bool AnimationControllerHandler::accepts(const std::string& extension) const {
    return extension == ".anc";
}

duk::resource::Handle<AnimationController> AnimationControllerHandler::load_from_text(AnimationControllerPool* pool, const resource::Id& id, const std::string_view& text) {
    auto controller = std::make_shared<AnimationController>();
    duk::serial::read_json(text, *controller);
    return pool->insert(id, controller);
}

}// namespace duk::animation
