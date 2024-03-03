//
// Created by rov on 1/3/2024.
//

#ifndef DUK_RENDERER_SPRITE_RENDERER_H
#define DUK_RENDERER_SPRITE_RENDERER_H

#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/sprite.h>

namespace duk::renderer {

struct SpriteRenderer {
    SpriteResource sprite;
    MaterialResource material;
};

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::SpriteRenderer& spriteRenderer) {
    visitor->template visit_member<duk::resource::Resource>(spriteRenderer.sprite, MemberDescription("sprite"));
    visitor->template visit_member<duk::resource::Resource>(spriteRenderer.material, MemberDescription("material"));
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::SpriteRenderer& spriteRenderer) {
    solver->solve(spriteRenderer.sprite);
    solver->solve(spriteRenderer.material);
}

}// namespace duk::resource

#endif//DUK_RENDERER_SPRITE_RENDERER_H
