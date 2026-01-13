//
// Created by rov on 1/3/2024.
//

#ifndef DUK_RENDERER_SPRITE_RENDERER_H
#define DUK_RENDERER_SPRITE_RENDERER_H

#include <duk_resource/handle.h>

#include <duk_renderer/material/material.h>
#include <duk_renderer/sprite/sprite.h>

namespace duk::renderer {

struct SpriteRenderer {
    SpriteResource sprite;
    uint32_t index;
};

void update_sprite_renderer(duk::tools::Globals* globals, const duk::objects::Component<SpriteRenderer>& spriteRenderer);

}// namespace duk::renderer

namespace duk::type {
// clang-format off
template<>
struct Type<duk::renderer::SpriteRenderer> : Class<duk::renderer::SpriteRenderer,
    Member<"sprite", &duk::renderer::SpriteRenderer::sprite>,
    Member<"index", &duk::renderer::SpriteRenderer::index>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_RENDERER_SPRITE_RENDERER_H
