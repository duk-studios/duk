//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_POOL_H
#define DUK_RENDERER_SPRITE_POOL_H

#include <duk_renderer/resources/sprite.h>

namespace duk::renderer {

struct SpritePoolCreateInfo {
    ImagePool* imagePool;
};

class SpritePool : public duk::resource::Pool<SpriteResource> {
public:

    explicit SpritePool(const SpritePoolCreateInfo& spritePoolCreateInfo);

    DUK_NO_DISCARD SpriteResource create(duk::resource::Id resourceId, const duk::renderer::SpriteDataSource* spriteDataSource);

    DUK_NO_DISCARD SpriteResource white_square() const;

private:
    ImagePool* m_imagePool;
    SpriteResource m_whiteSquareSprite;
};

}

#endif //DUK_RENDERER_SPRITE_POOL_H
