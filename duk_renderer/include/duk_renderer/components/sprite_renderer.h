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

void update_sprite_renderer(const Renderer* renderer, const duk::objects::Component<SpriteRenderer>& spriteRenderer);

}// namespace duk::renderer

namespace duk::serial {

template<>
inline void from_json<duk::renderer::SpriteRenderer>(const rapidjson::Value& json, duk::renderer::SpriteRenderer& spriteRenderer) {
    from_json_member(json, "sprite", spriteRenderer.sprite);
    from_json_member(json, "index", spriteRenderer.index);
}

template<>
inline void to_json<duk::renderer::SpriteRenderer>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::SpriteRenderer& spriteRenderer) {
    to_json_member(document, json, "sprite", spriteRenderer.sprite);
    to_json_member(document, json, "index", spriteRenderer.index);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::SpriteRenderer& spriteRenderer) {
    solver->solve(spriteRenderer.sprite);
}

}// namespace duk::resource

#endif//DUK_RENDERER_SPRITE_RENDERER_H
