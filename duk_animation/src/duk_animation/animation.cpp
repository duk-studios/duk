//
// Created by Ricardo on 25/04/2024.
//

#include <duk_animation/animation.h>

namespace duk::animation {

Animation::Animation()
    : m_sampleRate(0) {
}

void Animation::evaluate(duk::objects::Object& object, const float time) const {
    if (m_sampleRate == 0) {
        duk::log::warn("Animation with sample rate of 0, skipping");
        return;
    }

    const uint32_t sample = time * m_sampleRate;

    for (auto& property : m_properties) {
        property->evaluate(object, sample);
    }
}

uint32_t Animation::samples() const {
    const auto it = std::ranges::max_element(m_properties, [](const auto& lhs, const auto& rhs) -> bool {
        return lhs->samples() < rhs->samples();
    });
    return (*it)->samples();
}

float Animation::duration() const {
    return samples() / m_sampleRate;
}

std::vector<std::unique_ptr<Property>>::iterator Animation::begin() {
    return m_properties.begin();
}

std::vector<std::unique_ptr<Property>>::iterator Animation::end() {
    return m_properties.end();
}

}
