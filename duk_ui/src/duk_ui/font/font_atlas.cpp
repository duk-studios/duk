//
// Created by Ricardo on 30/03/2024.
//

#include <duk_ui/font/font_atlas.h>

#include <duk_rhi/image_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::rhi {

template<>
inline PixelFormat pixel_format_of<uint8_t>() {
    return PixelFormat::R8U;
}

}// namespace duk::rhi

namespace duk::ui {

FontAtlas::FontAtlas(const FontAtlasCreateInfo& fontAtlasCreateInfo)
    : m_fontSize(fontAtlasCreateInfo.fontSize) {
    duk::rhi::ImageDataSourceT<uint8_t> imageDataSource(fontAtlasCreateInfo.bitmap, fontAtlasCreateInfo.bitmapSize.x, fontAtlasCreateInfo.bitmapSize.y);
    imageDataSource.update_hash();

    duk::renderer::ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.rhi = fontAtlasCreateInfo.rhi;
    imageCreateInfo.commandQueue = fontAtlasCreateInfo.commandQueue;
    imageCreateInfo.imageDataSource = &imageDataSource;

    m_image = std::make_shared<duk::renderer::Image>(imageCreateInfo);

    for (auto& glyph: fontAtlasCreateInfo.glyphs) {
        m_glyphs[glyph.chr] = glyph;
    }
}

FontAtlas::~FontAtlas() = default;

duk::renderer::ImageResource FontAtlas::image() const {
    return m_image;
}

bool FontAtlas::find_glyph(char chr, Glyph& glyph, uint32_t fontSize) const {
    auto it = m_glyphs.find(chr);
    if (it != m_glyphs.end()) {
        glyph = it->second;
        scale_for(glyph, fontSize);
        return true;
    }
    glyph = m_glyphs.at(' ');
    scale_for(glyph, fontSize);
    return false;
}

void FontAtlas::scale_for(Glyph& glyph, uint32_t fontSize) const {
    auto scale = (float)fontSize / (float)m_fontSize;
    glyph.metrics.advance *= scale;
    glyph.metrics.size = glm::vec2(glyph.metrics.size) * scale;
    glyph.metrics.bearing = glm::vec2(glyph.metrics.bearing) * scale;
}

}// namespace duk::ui
