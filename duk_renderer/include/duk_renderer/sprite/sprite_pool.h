//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_POOL_H
#define DUK_RENDERER_SPRITE_POOL_H

#include <duk_renderer/sprite/sprite.h>

namespace duk::renderer {

class SpritePool : public duk::resource::PoolT<SpriteResource> {};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_POOL_H
