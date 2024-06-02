//
// Created by rov on 1/3/2024.
//

#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/sprite/sprite_cache.h>
#include <duk_renderer/renderer.h>

#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>

namespace duk::renderer {

void update_sprite_renderer(const Renderer* renderer, const duk::objects::Component<SpriteRenderer>& spriteRenderer) {
    auto cache = renderer->sprite_cache();
    auto spriteObject = spriteRenderer.object();
    auto [meshSlot, materialSlot] = spriteObject.components<MeshSlot, MaterialSlot>();
    if (!meshSlot) {
        meshSlot = spriteObject.add<MeshSlot>();
    }
    if (!materialSlot) {
        materialSlot = spriteObject.add<MaterialSlot>();
    }
    materialSlot->material = cache->material_for(spriteRenderer->sprite.get());
    meshSlot->mesh = cache->mesh_for(spriteRenderer->sprite.get(), spriteRenderer->index);
}

}// namespace duk::renderer
