//
// Created by Ricardo on 28/03/2024.
//

#include <duk_renderer/font/freetype/freetype_font.h>
#include <duk_renderer/renderer.h>

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

    const uint32_t kFontSize = 128;

    error = FT_Set_Char_Size(m_face, kFontSize * 64, 0, 100, 0);
    if (error) {
        throw std::runtime_error(build_freetype_error_message("failed to set freetype char size", error));
    }

    struct Bitmap {
        std::vector<uint8_t> buffer;
        uint32_t width;
        uint32_t height;
    };

    std::vector<Glyph> glyphs;
    std::unordered_map<char, Bitmap> glyphBitmaps;

    const auto& charSet = detail::kDefaultChars;
    for (uint32_t i = 32; i < 127; i++) {
        const char chr = (char)i;

        error = FT_Load_Char(m_face, chr, FT_LOAD_DEFAULT);
        if (error) {
            duk::log::debug("failed to load char \'{}\', reason: ", chr, FT_Error_String(error));
            continue;
        }
        auto slot = m_face->glyph;

        // render twice, so we force freetype to use it's bsdf renderer.
        // Should guarantee a better quality
        error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
        if (error) {
            duk::log::debug("failed to render char \'{}\' bitmap, reason: {}", chr, FT_Error_String(error));
            continue;
        }

        // sdf disabled, for now
        // error = FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
        // if (error) {
        //     duk::log::debug("failed to render char \'{}\' sdf, reason: {}", chr, FT_Error_String(error));
        //     continue;
        // }

        Glyph glyph = {};
        glyph.chr = chr;
        glyph.metrics.size.x = slot->metrics.width >> 6;
        glyph.metrics.size.y = slot->metrics.height >> 6;
        glyph.metrics.bearing.x = slot->metrics.horiBearingX >> 6;
        glyph.metrics.bearing.y = slot->metrics.horiBearingY >> 6;
        glyph.metrics.advance = slot->metrics.horiAdvance >> 6;
        glyphs.emplace_back(glyph);

        if (!slot->bitmap.buffer) {
            glyphBitmaps.emplace(chr, Bitmap{});
            continue;
        }

        Bitmap bitmap = {};
        bitmap.width = slot->bitmap.width;
        bitmap.height = slot->bitmap.rows;
        bitmap.buffer.resize(bitmap.width * bitmap.height);
        std::memcpy(bitmap.buffer.data(), slot->bitmap.buffer, bitmap.buffer.size());
        glyphBitmaps.emplace(chr, std::move(bitmap));
    }

    // pack glyphs into an image (coordinates only)

    /// sort glyphs by height
    std::sort(glyphs.begin(), glyphs.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.metrics.size.y > rhs.metrics.size.y;
    });

    /// place each glyph on the upper right of the previous one (starting at the upper left)
    const int32_t kMaxWidth = 1024;
    glm::ivec2 pen(kMaxWidth, 0);

    for (auto& glyph: glyphs) {
        auto& bitmap = glyphBitmaps.at(glyph.chr);
        /// if a glyph would be outside of the determined maximum width of the image, wrap into next row
        if (pen.x + bitmap.width > kMaxWidth) {
            pen.x = 0;
            pen.y += bitmap.height + 1;
        }

        glyph.min.x = pen.x;
        glyph.min.y = pen.y - bitmap.height;
        glyph.max = glyph.min + glm::ivec2(bitmap.width, bitmap.height);
        pen.x += bitmap.width + 1;//one pixel offset
    }

    // create image buffer with necessary size
    glm::ivec2 imageSize = {kMaxWidth, pen.y};
    std::vector<uint8_t> image(imageSize.x * imageSize.y);

    // load and copy glyphs bitmap data into the image buffer
    for (auto& glyph: glyphs) {
        if (glyph.min == glyph.max) {
            // empty glyph
            continue;
        }

        auto& bitmap = glyphBitmaps.at(glyph.chr);

        // compute uvs
        glyph.uvMin = (glm::vec2)glyph.min / (glm::vec2)imageSize;
        glyph.uvMax = (glm::vec2)glyph.max / (glm::vec2)imageSize;
        std::swap(glyph.uvMin.y, glyph.uvMax.y);

        const auto bitmapSize = glm::ivec2(bitmap.width, bitmap.height);

        const auto& min = glyph.min;

        // copy bitmap into image
        for (auto y = 0; y < bitmapSize.y; y++) {
            for (auto x = 0; x < bitmapSize.x; x++) {
                auto srcIndex = x + (y * bitmapSize.x);
                auto dst = min + glm::ivec2(x, y);
                auto dstIndex = dst.x + (dst.y * imageSize.x);

                FT_Byte pixel = bitmap.buffer[srcIndex];

                // use at for safe access (for now)
                image.at(dstIndex) = pixel;
            }
        }
    }

    // create font atlas
    auto renderer = freetypeFontCreateInfo.renderer;

    FontAtlasCreateInfo fontAtlasCreateInfo = {};
    fontAtlasCreateInfo.glyphs = std::move(glyphs);
    fontAtlasCreateInfo.bitmap = std::move(image);
    fontAtlasCreateInfo.bitmapSize = imageSize;
    fontAtlasCreateInfo.rhi = renderer->rhi();
    fontAtlasCreateInfo.commandQueue = renderer->main_command_queue();
    fontAtlasCreateInfo.fontSize = kFontSize;

    m_atlas = std::make_unique<FontAtlas>(fontAtlasCreateInfo);
}

FreetypeFont::~FreetypeFont() {
    auto error = FT_Done_Face(m_face);
    if (error) {
        duk::log::fatal(build_freetype_error_message("failed to delete freetype face", error));
    }
}

FontAtlas* FreetypeFont::atlas() {
    return m_atlas.get();
}

}// namespace duk::renderer