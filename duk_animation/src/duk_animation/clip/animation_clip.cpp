//
// Created by Ricardo on 25/04/2024.
//

#include <duk_animation/clip/animation_clip.h>

namespace duk::animation {

AnimationClip::AnimationClip()
    : m_sampleRate(0) {
}

void AnimationClip::evaluate(const duk::objects::Object& object, const float time) const {
    if (m_sampleRate == 0) {
        duk::log::warn("AnimationClip with sample rate of 0, skipping");
        return;
    }

    const uint32_t sample = time * m_sampleRate;

    for (auto& property : m_properties) {
        property->evaluate(object, sample);
    }
}

uint32_t AnimationClip::samples() const {
    const auto it = std::ranges::max_element(m_properties, [](const auto& lhs, const auto& rhs) -> bool {
        return lhs->samples() < rhs->samples();
    });
    return (*it)->samples();
}

float AnimationClip::duration() const {
    return samples() / m_sampleRate;
}

std::vector<std::unique_ptr<Property>>::iterator AnimationClip::begin() {
    return m_properties.begin();
}

std::vector<std::unique_ptr<Property>>::iterator AnimationClip::end() {
    return m_properties.end();
}

}
