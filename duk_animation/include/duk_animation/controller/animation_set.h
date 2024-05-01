//
// Created by Ricardo on 28/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_SET_H
#define DUK_ANIMATION_ANIMATION_SET_H

#include <duk_animation/clip/animation_clip.h>
#include <duk_animation/controller/animation_state.h>
#include <duk_animation/controller/animation_transition.h>

namespace duk::animation {

struct AnimationEntry {
    std::string name;
    AnimationClipResource clip;
    std::vector<AnimationTransition> transitions;
};

class AnimationSet {
public:
    const AnimationEntry* front() const;

    const AnimationEntry* at(const std::string_view& name) const;

    std::vector<AnimationEntry>::iterator begin();

    std::vector<AnimationEntry>::iterator end();

    friend void serial::from_json<AnimationSet>(const rapidjson::Value& json, AnimationSet& animationSet);

    friend void serial::to_json<AnimationSet>(rapidjson::Document& document, rapidjson::Value& json, const AnimationSet& animationSet);

private:
    std::vector<AnimationEntry> m_animations;
};

using AnimationSetResource = duk::resource::ResourceT<AnimationSet>;

}// namespace duk::animation

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::AnimationEntry& animationEntry) {
    from_json_member(json, "name", animationEntry.name);
    from_json_member(json, "clip", animationEntry.clip);
    from_json_member(json, "transitions", animationEntry.transitions);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::AnimationEntry& animationEntry) {
    to_json_member(document, json, "name", animationEntry.name);
    to_json_member(document, json, "clip", animationEntry.clip);
    to_json_member(document, json, "transitions", animationEntry.transitions);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::animation::AnimationSet& animationSet) {
    from_json(json, animationSet.m_animations);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::AnimationSet& animationSet) {
    to_json(document, json, animationSet.m_animations);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::AnimationSet& animationSet) {
    for (auto& animation: animationSet) {
        solver->solve(animation.clip);
    }
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATION_SET_H
