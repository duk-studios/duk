//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_RENDERER_FREETYPE_FONT_H
#define DUK_RENDERER_FREETYPE_FONT_H

#include <duk_renderer/font/font.h>
#include <duk_renderer/font/freetype/freetype_import.h>

namespace duk::renderer {

struct FreetypeFontCreateInfo {
    std::vector<uint8_t> fontData;
    FT_Library library;
    Renderer* renderer;
};

class FreetypeFont : public Font {
public:
    explicit FreetypeFont(const FreetypeFontCreateInfo& freetypeFontCreateInfo);

    ~FreetypeFont() override;

    FontAtlas* atlas() override;

private:
    std::vector<uint8_t> m_fontData;
    FT_Face m_face;
    std::unique_ptr<FontAtlas> m_atlas;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_FREETYPE_FONT_H
