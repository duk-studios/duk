//
// Created by Ricardo on 26/04/2024.
//

#include <duk_animation/animation_handler.h>

#include <duk_tools/file.h>

namespace duk::animation {

AnimationHandler::AnimationHandler() :
    ResourceHandlerT("ani") {
}

bool AnimationHandler::accepts(const std::string& extension) const {
    return extension == ".ani";
}

void AnimationHandler::load(AnimationPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    const auto content = duk::tools::load_text(path);

    const auto animation = std::make_shared<Animation>();
    duk::serial::read_json(content, *animation);

    pool->insert(id, animation);
}
}
