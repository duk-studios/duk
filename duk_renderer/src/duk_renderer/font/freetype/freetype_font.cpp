//
// Created by Ricardo on 28/03/2024.
//

#include <duk_renderer/font/freetype/freetype_font.h>

#include <duk_log/log.h>

namespace duk::renderer {

namespace detail {

static const std::string kDefaultChars = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&*(){}[]:.;?";

}

FreetypeFont::FreetypeFont(const FreetypeFontCreateInfo& freetypeFontCreateInfo)
    : m_fontData(std::move(freetypeFontCreateInfo.fontData)) {
    auto error = FT_New_Memory_Face(freetypeFontCreateInfo.library, (const FT_Byte*)m_fontData.data(), m_fontData.size(), 0, &m_face);

    if (error) {
        throw std::runtime_error(build_freetype_error_message("failed to create freetype face", error));
    }
}

FreetypeFont::~FreetypeFont() {
    auto error = FT_Done_Face(m_face);
    if (error) {
        duk::log::fatal(build_freetype_error_message("failed to delete freetype face", error));
    }
}

FontAtlas* FreetypeFont::atlas(uint32_t fontSize, duk::rhi::RHI* rhi, duk::rhi::CommandQueue* commandQueue) {
    {
        auto it = m_atlases.find(fontSize);
        if (it != m_atlases.end() && it->second) {
            return it->second.get();
        }
    }

    auto error = FT_Set_Char_Size(m_face, fontSize * 64, 0, 100, 0);
    if (error) {
        throw std::runtime_error(build_freetype_error_message("failed to set freetype char size", error));
    }

    std::vector<Glyph> glyphs;

    const auto& charSet = detail::kDefaultChars;
    for (uint32_t i = 0; i < charSet.size(); i++) {
        const auto chr = charSet[i];

        error = FT_Load_Char(m_face, chr, FT_LOAD_BITMAP_METRICS_ONLY);
        if (error) {
            duk::log::debug("failed to load char \'{}\', reason: ", chr, FT_Error_String(error));
            continue;
        }
        auto slot = m_face->glyph;

        Glyph glyph = {};
        glyph.chr = chr;
        glyph.metrics.size.x = slot->metrics.width >> 6;
        glyph.metrics.size.y = slot->metrics.height >> 6;
        glyph.metrics.bearing.x = slot->metrics.horiBearingX >> 6;
        glyph.metrics.bearing.y = slot->metrics.horiBearingY >> 6;
        glyph.metrics.advance = slot->metrics.horiAdvance >> 6;
        glyphs.emplace_back(glyph);
    }

    // pack glyphs into an image (coordinates only)

    /// sort glyphs by height
    std::sort(glyphs.begin(), glyphs.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.metrics.size.y > rhs.metrics.size.y;
    });

    /// place each glyph on the upper right of the previous one (starting at the upper left)
    const int32_t kMaxWidth = 256;
    glm::ivec2 pen(kMaxWidth, 0);

    for (auto& glyph: glyphs) {
        /// if a glyph would be outside of the determined maximum width of the image, wrap into next row
        if (pen.x + glyph.metrics.size.x > kMaxWidth) {
            pen.x = 0;
            pen.y += glyph.metrics.size.y;
        }

        glyph.min.x = pen.x;
        glyph.min.y = pen.y - glyph.metrics.size.y;
        glyph.max = glyph.min + glyph.metrics.size;
        pen.x += glyph.metrics.size.x;
    }

    // create image buffer with necessary size
    glm::ivec2 imageSize = {kMaxWidth, pen.y};
    std::vector<uint8_t> image(imageSize.x * imageSize.y);

    // load and copy glyphs bitmap data into the image buffer
    for (auto& glyph: glyphs) {
        error = FT_Load_Char(m_face, glyph.chr, FT_LOAD_RENDER);
        if (error) {
            duk::log::debug("failed to load/render char \'{}\', reason: ", glyph.chr, FT_Error_String(error));
            continue;
        }

        auto slot = m_face->glyph;

        // compute uvs
        glyph.uvMin = (glm::vec2)glyph.min / (glm::vec2)imageSize;
        glyph.uvMax = (glm::vec2)glyph.max / (glm::vec2)imageSize;
        std::swap(glyph.uvMin.y, glyph.uvMax.y);

        const auto& size = glyph.metrics.size;
        const auto& min = glyph.min;

        // copy bitmap into image
        for (auto y = 0; y < size.y; y++) {
            for (auto x = 0; x < size.x; x++) {
                auto srcIndex = x + (y * size.x);
                auto dst = min + glm::ivec2(x, y);
                auto dstIndex = dst.x + (dst.y * imageSize.x);

                // use at for safe access (for now)
                image.at(dstIndex) = slot->bitmap.buffer[srcIndex];
            }
        }
    }

    // create font atlas

    FontAtlasCreateInfo fontAtlasCreateInfo = {};
    fontAtlasCreateInfo.glyphs = std::move(glyphs);
    fontAtlasCreateInfo.bitmap = std::move(image);
    fontAtlasCreateInfo.bitmapSize = imageSize;
    fontAtlasCreateInfo.rhi = rhi;
    fontAtlasCreateInfo.commandQueue = commandQueue;

    auto& atlas = m_atlases[fontSize];

    atlas = std::make_unique<FontAtlas>(fontAtlasCreateInfo);

    return atlas.get();
}

}// namespace duk::renderer