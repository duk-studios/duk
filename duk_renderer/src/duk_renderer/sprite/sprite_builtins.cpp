//
// Created by Ricardo on 26/04/2026.
//

#include <duk_renderer/sprite/sprite_builtins.h>
#include <duk_renderer/image/image_builtins.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<Sprite> create_pixel_sprite(const duk::resource::Handle<Image>& image) {
    SpriteAtlasData spriteAtlasData = {};
    spriteAtlasData.image = image;
    spriteAtlasData.pixelsPerUnit = 1.0f;
    spriteAtlasData.sprites.push_back({
            .min = {0, 0},
            .max = {1, 1},
            .pivot = {0.5f, 0.5f},
    });

    SpriteAtlasCreateInfo spriteAtlasCreateInfo = {};
    spriteAtlasCreateInfo.spriteAtlasData = &spriteAtlasData;

    return std::make_shared<Sprite>(spriteAtlasCreateInfo);
}

}// namespace detail

SpriteBuiltins::SpriteBuiltins(const SpriteBuiltinsCreateInfo& spriteBuiltinsCreateInfo) {
    const auto pools = spriteBuiltinsCreateInfo.pools;
    const auto imageBuiltins = spriteBuiltinsCreateInfo.imageBuiltins;

    m_white = pools->insert(kWhiteSpriteId, detail::create_pixel_sprite(imageBuiltins->white()));
    m_black = pools->insert(kBlackSpriteId, detail::create_pixel_sprite(imageBuiltins->black()));
    m_magenta = pools->insert(kMagentaSpriteId, detail::create_pixel_sprite(imageBuiltins->magenta()));
}

SpriteResource SpriteBuiltins::white() const {
    return m_white;
}

SpriteResource SpriteBuiltins::black() const {
    return m_black;
}

SpriteResource SpriteBuiltins::magenta() const {
    return m_magenta;
}

}// namespace duk::renderer
