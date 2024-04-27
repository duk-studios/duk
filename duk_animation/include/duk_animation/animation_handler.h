//
// Created by Ricardo on 26/04/2024.
//

#ifndef DUK_ANIMATION_ANIMATION_HANDLER_H
#define DUK_ANIMATION_ANIMATION_HANDLER_H

#include <duk_animation/animation_pool.h>

#include <duk_resource/handler.h>

namespace duk::animation {

class AnimationHandler : public duk::resource::ResourceHandlerT<AnimationPool> {
public:
    AnimationHandler();

    bool accepts(const std::string& extension) const override;

protected:
    void load(AnimationPool* pool, const resource::Id& id, const std::filesystem::path& path) override;
};

}

#endif //DUK_ANIMATION_ANIMATION_HANDLER_H
