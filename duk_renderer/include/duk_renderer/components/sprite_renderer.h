//
// Created by rov on 1/3/2024.
//

#ifndef DUK_RENDERER_SPRITE_RENDERER_H
#define DUK_RENDERER_SPRITE_RENDERER_H

#include <duk_renderer/resources/sprite.h>
#include <duk_renderer/resources/materials/material.h>

namespace duk::renderer {

struct SpriteRenderer {
    SpriteResource sprite;
    MaterialResource material;
};

}

namespace duk::json {

template<>
inline void from_json<duk::renderer::SpriteRenderer>(const rapidjson::Value& jsonObject, duk::renderer::SpriteRenderer& object) {
    object.sprite = from_json_member<duk::resource::Id>(jsonObject, "sprite");
    object.material = from_json_member<duk::resource::Id>(jsonObject, "material");
}

}

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::SpriteRenderer& spriteRenderer) {
    solver->solve(spriteRenderer.sprite);
    solver->solve(spriteRenderer.material);
}

}

#endif //DUK_RENDERER_SPRITE_RENDERER_H
