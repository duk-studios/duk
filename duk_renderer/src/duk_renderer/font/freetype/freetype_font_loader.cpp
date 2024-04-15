//
// Created by Ricardo on 29/03/2024.
//

#include <duk_renderer/font/freetype/freetype_font.h>
#include <duk_renderer/font/freetype/freetype_font_loader.h>

#include <duk_tools/file.h>

namespace duk::renderer {

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

std::shared_ptr<Font> FreetypeFontLoader::load(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument(fmt::format("failed to load freetype font, file ({}) does not exist", path.string()));
    }

    FreetypeFontCreateInfo freetypeFontCreateInfo = {};
    freetypeFontCreateInfo.library = m_library;
    freetypeFontCreateInfo.fontData = duk::tools::load_bytes(path);

    return std::make_shared<FreetypeFont>(freetypeFontCreateInfo);
}

}// namespace duk::renderer