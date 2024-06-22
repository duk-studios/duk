//
// Created by Ricardo on 26/04/2024.
//

#include <duk_animation/clip/animation_clip_handler.h>

#include <duk_serial/json/serializer.h>

#include <duk_tools/file.h>

namespace duk::animation {

AnimationClipHandler::AnimationClipHandler()
    : TextResourceHandlerT("ani") {
}

bool AnimationClipHandler::accepts(const std::string& extension) const {
    return extension == ".ani";
}

duk::resource::Handle<AnimationClip> AnimationClipHandler::load_from_text(AnimationClipPool* pool, const resource::Id& id, const std::string_view& text) {
    const auto animation = std::make_shared<AnimationClip>();
    duk::serial::read_json(text, *animation);

    return pool->insert(id, animation);
}

}// namespace duk::animation
