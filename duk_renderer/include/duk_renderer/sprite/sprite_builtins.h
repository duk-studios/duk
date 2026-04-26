//
// Created by Ricardo on 26/04/2026.
//

#ifndef DUK_RENDERER_SPRITE_BUILTINS_H
#define DUK_RENDERER_SPRITE_BUILTINS_H

#include <duk_renderer/sprite/sprite.h>
#include <duk_resource/pool.h>

namespace duk::renderer {

// 20,001 - 30,000 - reserved for built-in sprites
static constexpr duk::resource::Id kWhiteSpriteId(20001);
static constexpr duk::resource::Id kBlackSpriteId(20002);
static constexpr duk::resource::Id kMagentaSpriteId(20003);

class ImageBuiltins;

struct SpriteBuiltinsCreateInfo {
    duk::resource::Pools* pools;
    ImageBuiltins* imageBuiltins;
};

class SpriteBuiltins {
public:
    explicit SpriteBuiltins(const SpriteBuiltinsCreateInfo& spriteBuiltinsCreateInfo);

    DUK_NO_DISCARD SpriteResource white() const;

    DUK_NO_DISCARD SpriteResource black() const;

    DUK_NO_DISCARD SpriteResource magenta() const;

private:
    SpriteResource m_white;
    SpriteResource m_black;
    SpriteResource m_magenta;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_BUILTINS_H
