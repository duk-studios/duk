//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_POOL_H
#define DUK_RENDERER_SPRITE_POOL_H

#include <duk_renderer/sprite/sprite.h>

namespace duk::renderer {

// 20,001 - 30,000 - reserved for built-in sprites
static constexpr duk::resource::Id kWhiteSquareSpriteId(20001);

struct SpritePoolCreateInfo {
    ImagePool* imagePool;
};

class SpritePool : public duk::resource::PoolT<SpriteResource> {
public:
    explicit SpritePool(const SpritePoolCreateInfo& spritePoolCreateInfo);

    DUK_NO_DISCARD SpriteResource create(duk::resource::Id resourceId, const duk::renderer::SpriteDataSource* spriteDataSource);

    DUK_NO_DISCARD SpriteResource white_square() const;

private:
    ImagePool* m_imagePool;
    SpriteResource m_whiteSquareSprite;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_POOL_H
