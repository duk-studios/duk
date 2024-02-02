//
// Created by rov on 1/3/2024.
//

#ifndef DUK_RENDERER_SPRITE_RENDERER_H
#define DUK_RENDERER_SPRITE_RENDERER_H

#include <duk_renderer/resources/sprite.h>
#include <duk_renderer/resources/materials/sprite_material.h>

namespace duk::renderer {

struct SpriteRenderer {
    SpriteResource sprite;
    SpriteMaterialResource material;
};

}

#endif //DUK_RENDERER_SPRITE_RENDERER_H
