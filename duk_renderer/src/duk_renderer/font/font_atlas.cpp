//
// Created by Ricardo on 30/03/2024.
//

#include <duk_renderer/font/font_atlas.h>

#include <duk_rhi/rhi.h>
#include <duk_rhi/image_data_source.h>

namespace duk::rhi {

template<>
inline PixelFormat pixel_format_of<uint8_t>() {
    return PixelFormat::R8U;
}

}

namespace duk::renderer {

FontAtlas::FontAtlas(const FontAtlasCreateInfo& fontAtlasCreateInfo) {
    duk::rhi::ImageDataSourceT<uint8_t> imageDataSource(fontAtlasCreateInfo.bitmap, fontAtlasCreateInfo.bitmapSize.x, fontAtlasCreateInfo.bitmapSize.y);
    imageDataSource.update_hash();

    ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.rhi = fontAtlasCreateInfo.rhi;
    imageCreateInfo.commandQueue = fontAtlasCreateInfo.commandQueue;
    imageCreateInfo.imageDataSource = &imageDataSource;

    m_image = std::make_shared<Image>(imageCreateInfo);

    for (auto& glyph : fontAtlasCreateInfo.glyphs) {
        m_glyphs[glyph.chr] = glyph;
    }
}

FontAtlas::~FontAtlas() = default;

ImageResource FontAtlas::image() const {
    return m_image;
}
} // duk::renderer