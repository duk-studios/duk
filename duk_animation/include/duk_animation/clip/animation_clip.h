//
// Created by Ricardo on 25/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_CLIP_H
#define DUK_ANIMATION_ANIMATION_CLIP_H

#include <duk_animation/property.h>

#include <duk_objects/objects.h>

#include <duk_resource/resource.h>

namespace duk::animation {

class AnimationClip {
public:

    AnimationClip();

    void evaluate(duk::objects::Object& object, float time) const;

    uint32_t samples() const;

    float duration() const;

    std::vector<std::unique_ptr<Property>>::iterator begin();

    std::vector<std::unique_ptr<Property>>::iterator end();

    friend void serial::from_json<AnimationClip>(const rapidjson::Value& json, AnimationClip& animation);

    friend void serial::to_json<AnimationClip>(rapidjson::Document& document, rapidjson::Value& json, const AnimationClip& animation);

private:
    float m_sampleRate;
    std::vector<std::unique_ptr<Property>> m_properties;
};

using AnimationClipResource = duk::resource::ResourceT<AnimationClip>;

}

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, std::vector<std::unique_ptr<duk::animation::Property>>& properties) {
    for (const auto& propertyJson : json.GetArray()) {
        from_json(propertyJson, properties.emplace_back());
    }
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const std::vector<std::unique_ptr<duk::animation::Property>>& properties) {
    auto array = json.SetArray().GetArray();
    for (const auto& property : properties) {
        rapidjson::Value propertyJson;
        to_json(document, propertyJson, property);
        array.PushBack(propertyJson, document.GetAllocator());
    }
}


template<>
inline void from_json(const rapidjson::Value& json, duk::animation::AnimationClip& animation) {
    from_json_member(json, "sampleRate", animation.m_sampleRate);
    from_json_member(json, "properties", animation.m_properties);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::AnimationClip& animation) {
    to_json_member(document, json, "sampleRate", animation.m_sampleRate);
    to_json_member(document, json, "properties", animation.m_properties);
}

}

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::AnimationClip& animation) {
    for (auto& property : animation) {
        solver->solve(*property);
    }
}

}

#endif //DUK_ANIMATION_ANIMATION_CLIP_H
