//
// Created by Ricardo on 23/04/2024.
//

#ifndef DUK_RENDERER_SPRITE_HANDLER_H
#define DUK_RENDERER_SPRITE_HANDLER_H

#include <duk_renderer/sprite/sprite.h>

#include <duk_resource/handler.h>

namespace duk::renderer {

class SpriteHandler : public duk::resource::TextHandlerT<Sprite> {
public:
    SpriteHandler();

    bool accepts(const std::string& extension) const override;

protected:
    std::shared_ptr<Sprite> load_from_text(duk::tools::Globals* globals, const std::string_view& text) override;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_HANDLER_H
