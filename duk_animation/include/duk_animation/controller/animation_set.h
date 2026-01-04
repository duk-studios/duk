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

    friend struct duk::type::Type<AnimationSet>;

private:
    std::vector<AnimationEntry> m_animations;
};

using AnimationSetResource = duk::resource::Handle<AnimationSet>;

}// namespace duk::animation

namespace duk::type {

template<>
struct Type<duk::animation::AnimationEntry> : Class<duk::animation::AnimationEntry,
    Member<"name", &duk::animation::AnimationEntry::name>,
    Member<"clip", &duk::animation::AnimationEntry::clip>,
    Member<"transitions", &duk::animation::AnimationEntry::transitions>> {
};

template<>
struct Type<duk::animation::AnimationSet> : Class<duk::animation::AnimationSet,
    Member<"animations", &duk::animation::AnimationSet::m_animations>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::AnimationSet& animationSet) {
    for (auto& animation: animationSet) {
        solver->solve(animation.clip);
    }
}

}// namespace duk::resource

#endif//DUK_ANIMATION_ANIMATION_SET_H
