//
// Created by Ricardo on 23/04/2024.
//

#ifndef DUK_RENDERER_SPRITE_HANDLER_H
#define DUK_RENDERER_SPRITE_HANDLER_H

#include <duk_renderer/sprite/sprite_pool.h>

#include <duk_resource/handler.h>

namespace duk::renderer {

class SpriteHandler : public duk::resource::ResourceHandlerT<SpritePool> {
public:
    SpriteHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<Sprite> load(SpritePool* pool, const resource::Id& id, const std::filesystem::path& path) override;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_HANDLER_H
