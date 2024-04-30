//
// Created by Ricardo on 28/04/2024.
//

#include <duk_animation/controller/animation_set.h>

namespace duk::animation {

const AnimationEntry* AnimationSet::front() const {
    return &m_animations.front();
}

const AnimationEntry* AnimationSet::at(const std::string_view& name) const {
    auto it = std::ranges::find_if(m_animations, [&name](const AnimationEntry& entry) {
        return entry.name == name;
    });
    return &(*it);
}

std::vector<AnimationEntry>::iterator AnimationSet::begin() {
    return m_animations.begin();
}

std::vector<AnimationEntry>::iterator AnimationSet::end() {
    return m_animations.end();
}

}// namespace duk::animation
