//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_UI_FREETYPE_FONT_LOADER_H
#define DUK_UI_FREETYPE_FONT_LOADER_H

#include <duk_ui/font/font_handler.h>
#include <duk_ui/font/freetype/freetype_import.h>

namespace duk::ui {

class FreetypeFontLoader : public FontLoader {
public:
    FreetypeFontLoader();

    ~FreetypeFontLoader() override;

    bool accepts(const std::filesystem::path& extension) override;

    bool accepts(const void* data, size_t size) override;

    std::shared_ptr<Font> load(const void* data, size_t size) override;

private:
    FT_Library m_library;
};

}// namespace duk::ui

#endif//DUK_UI_FREETYPE_FONT_LOADER_H
