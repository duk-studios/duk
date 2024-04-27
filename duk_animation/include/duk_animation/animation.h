//
// Created by Ricardo on 25/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_H
#define DUK_ANIMATION_ANIMATION_H

#include <duk_animation/property.h>

#include <duk_objects/objects.h>

#include <duk_resource/resource.h>

namespace duk::animation {

class Animation {
public:

    Animation();

    void evaluate(duk::objects::Object& object, float time) const;

    uint32_t samples() const;

    float duration() const;

    std::vector<std::unique_ptr<Property>>::iterator begin();

    std::vector<std::unique_ptr<Property>>::iterator end();

    friend void serial::from_json<Animation>(const rapidjson::Value& json, Animation& animation);

    friend void serial::to_json<Animation>(rapidjson::Document& document, rapidjson::Value& json, const Animation& animation);

private:
    float m_sampleRate;
    std::vector<std::unique_ptr<Property>> m_properties;
};

using AnimationResource = duk::resource::ResourceT<Animation>;

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
inline void from_json(const rapidjson::Value& json, duk::animation::Animation& animation) {
    from_json_member(json, "sampleRate", animation.m_sampleRate);
    from_json_member(json, "properties", animation.m_properties);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::Animation& animation) {
    to_json_member(document, json, "sampleRate", animation.m_sampleRate);
    to_json_member(document, json, "properties", animation.m_properties);
}

}

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::Animation& animation) {
    for (auto& property : animation) {
        solver->solve(*property);
    }
}

}

#endif //DUK_ANIMATION_ANIMATION_H
