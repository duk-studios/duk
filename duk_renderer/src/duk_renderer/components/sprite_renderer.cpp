//
// Created by rov on 1/3/2024.
//

#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/sprite/sprite_cache.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/builtins.h>

#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>
#include <duk_renderer/sprite/sprite_mesh.h>

namespace duk::renderer {

void update_sprite_renderer(const duk::tools::Globals& globals, const duk::objects::Component<SpriteRenderer>& spriteRenderer) {
    auto sprite = spriteRenderer->sprite;
    if (!sprite.valid()) {
        auto builtins = globals.get<Builtins>();
        sprite = builtins->sprites()->magenta();
    }
    auto renderer = globals.get<Renderer>();
    auto cache = renderer->sprite_cache();
    auto spriteObject = spriteRenderer.object();
    auto [meshSlot, materialSlot] = spriteObject.components<MeshSlot, MaterialSlot>();
    if (!meshSlot) {
        meshSlot = spriteObject.add<MeshSlot>();
    }
    if (!materialSlot) {
        materialSlot = spriteObject.add<MaterialSlot>();
    }
    materialSlot->material = cache->material_for(globals, *sprite);
    meshSlot->mesh = cache->mesh_for(globals, *sprite, spriteRenderer->index);
}

}// namespace duk::renderer
