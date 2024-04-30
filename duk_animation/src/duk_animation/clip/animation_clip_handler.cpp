//
// Created by Ricardo on 26/04/2024.
//

#include <duk_animation/clip/animation_clip_handler.h>

#include <duk_tools/file.h>

namespace duk::animation {

AnimationClipHandler::AnimationClipHandler()
    : ResourceHandlerT("ani") {
}

bool AnimationClipHandler::accepts(const std::string& extension) const {
    return extension == ".ani";
}

void AnimationClipHandler::load(AnimationClipPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    const auto content = duk::tools::load_text(path);

    const auto animation = std::make_shared<AnimationClip>();
    duk::serial::read_json(content, *animation);

    pool->insert(id, animation);
}
}// namespace duk::animation
