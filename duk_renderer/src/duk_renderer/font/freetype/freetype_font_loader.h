//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_RENDERER_FREETYPE_FONT_LOADER_H
#define DUK_RENDERER_FREETYPE_FONT_LOADER_H

#include <duk_renderer/font/font_handler.h>
#include <duk_renderer/font/freetype/freetype_import.h>

namespace duk::renderer {

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

}// namespace duk::renderer

#endif//DUK_RENDERER_FREETYPE_FONT_LOADER_H
