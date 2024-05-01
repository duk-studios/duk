//
// Created by Ricardo on 30/04/2024.
//

#include <duk_animation/controller/animation_controller_handler.h>
#include <duk_tools/file.h>

namespace duk::animation {

AnimationControllerHandler::AnimationControllerHandler()
    : ResourceHandlerT("anc") {
}

bool AnimationControllerHandler::accepts(const std::string& extension) const {
    return extension == ".anc";
}

void AnimationControllerHandler::load(AnimationControllerPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::load_text(path);
    auto controller = std::make_shared<AnimationController>();
    duk::serial::read_json(content, *controller);
    pool->insert(id, controller);
}

}// namespace duk::animation
