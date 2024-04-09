//
// Created by Ricardo on 30/03/2024.
//

#include <duk_renderer/font/font_atlas.h>

#include <duk_rhi/image_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::rhi {

template<>
inline PixelFormat pixel_format_of<uint8_t>() {
    return PixelFormat::R8U;
}

}// namespace duk::rhi

namespace duk::renderer {

FontAtlas::FontAtlas(const FontAtlasCreateInfo& fontAtlasCreateInfo) {
    duk::rhi::ImageDataSourceT<uint8_t> imageDataSource(fontAtlasCreateInfo.bitmap, fontAtlasCreateInfo.bitmapSize.x, fontAtlasCreateInfo.bitmapSize.y);
    imageDataSource.update_hash();

    ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.rhi = fontAtlasCreateInfo.rhi;
    imageCreateInfo.commandQueue = fontAtlasCreateInfo.commandQueue;
    imageCreateInfo.imageDataSource = &imageDataSource;

    m_image = std::make_shared<Image>(imageCreateInfo);

    for (auto& glyph: fontAtlasCreateInfo.glyphs) {
        m_glyphs[glyph.chr] = glyph;
    }
}

FontAtlas::~FontAtlas() = default;

ImageResource FontAtlas::image() const {
    return m_image;
}

const duk::renderer::Glyph& duk::renderer::FontAtlas::glyph(char chr) const {
    auto it = m_glyphs.find(chr);
    if (it != m_glyphs.end()) {
        return it->second;
    }
    return m_glyphs.at('?');
}

}// namespace duk::renderer
