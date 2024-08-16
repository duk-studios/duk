//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_UI_FREETYPE_FONT_H
#define DUK_UI_FREETYPE_FONT_H

#include <duk_ui/font/font.h>
#include <duk_ui/font/freetype/freetype_import.h>

namespace duk::ui {

struct FreetypeFontCreateInfo {
    std::vector<uint8_t> fontData;
    FT_Library library;
};

class FreetypeFont : public Font {
public:
    explicit FreetypeFont(const FreetypeFontCreateInfo& freetypeFontCreateInfo);

    ~FreetypeFont() override;

    FontAtlas* atlas(const BuildAtlasParams& buildAtlasParams) override;

private:
    std::vector<uint8_t> m_fontData;
    FT_Face m_face;
    std::unordered_map<uint32_t, std::unique_ptr<FontAtlas>> m_atlases;
};

}// namespace duk::ui

#endif//DUK_UI_FREETYPE_FONT_H
