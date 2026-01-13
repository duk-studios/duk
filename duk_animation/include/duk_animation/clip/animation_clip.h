//
// Created by Ricardo on 25/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CLIP_H
#define DUK_ANIMATION_ANIMATION_CLIP_H

#include <duk_animation/clip/property.h>

#include <duk_objects/objects.h>

#include <duk_resource/handle.h>

namespace duk::animation {

class AnimationClip {
public:
    AnimationClip();

    void evaluate(const duk::objects::Object& object, float time) const;

    void evaluate(const duk::objects::Object& object, uint32_t sample) const;

    uint32_t samples() const;

    float duration() const;

    std::vector<std::unique_ptr<Property>>::iterator begin();

    std::vector<std::unique_ptr<Property>>::iterator end();

    friend struct duk::type::Type<AnimationClip>;

private:
    float m_sampleRate;
    std::vector<std::unique_ptr<Property>> m_properties;
};

using AnimationClipResource = duk::resource::Handle<AnimationClip>;

}// namespace duk::animation

namespace duk::type {
// clang-format off
template<>
struct Type<duk::animation::AnimationClip> : Class<duk::animation::AnimationClip,
    Member<"sampleRate", &duk::animation::AnimationClip::m_sampleRate>,
    Member<"properties", &duk::animation::AnimationClip::m_properties>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_ANIMATION_ANIMATION_CLIP_H
