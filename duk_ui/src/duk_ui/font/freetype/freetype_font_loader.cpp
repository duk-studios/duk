//
// Created by Ricardo on 29/03/2024.
//

#include <duk_ui/font/freetype/freetype_font.h>
#include <duk_ui/font/freetype/freetype_font_loader.h>

#include <duk_tools/file.h>

namespace duk::ui {

FreetypeFontLoader::FreetypeFontLoader() {
    auto error = FT_Init_FreeType(&m_library);
    if (error) {
        throw std::runtime_error(build_freetype_error_message("failed to initialize freetype library", error));
    }
}

FreetypeFontLoader::~FreetypeFontLoader() {
    FT_Done_FreeType(m_library);
}

bool FreetypeFontLoader::accepts(const std::filesystem::path& extension) {
    return extension == ".ttf";
}

bool FreetypeFontLoader::accepts(const void* data, size_t size) {
    // for now we only have ttf files, so we can just trust that the data is a valid ttf file
    // but if we have different font formats we might need to validate it here
    return true;
}

std::shared_ptr<Font> FreetypeFontLoader::load(const void* data, size_t size) {
    FreetypeFontCreateInfo freetypeFontCreateInfo = {};
    freetypeFontCreateInfo.library = m_library;
    freetypeFontCreateInfo.fontData.assign(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);

    return std::make_shared<FreetypeFont>(freetypeFontCreateInfo);
}

}// namespace duk::ui