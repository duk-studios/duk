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

template<>
struct Type<duk::renderer::SpriteRenderer> : Class<duk::renderer::SpriteRenderer,
    Member<"sprite", &duk::renderer::SpriteRenderer::sprite>,
    Member<"index", &duk::renderer::SpriteRenderer::index>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::SpriteRenderer& spriteRenderer) {
    solver->solve(spriteRenderer.sprite);
}

}// namespace duk::resource

#endif//DUK_RENDERER_SPRITE_RENDERER_H
