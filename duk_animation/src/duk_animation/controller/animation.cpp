//
// Created by Ricardo on 28/04/2024.
//

#include <duk_animation/controller/animation.h>

namespace duk::animation {

const Animation* find_animation(const std::vector<Animation>& animations, const std::string_view& name) {
    const auto it = std::ranges::find_if(animations, [&name](const Animation& entry) {
        return entry.name == name;
    });
    if (it == animations.end()) {
        return nullptr;
    }
    return &(*it);
}

}// namespace duk::animation
