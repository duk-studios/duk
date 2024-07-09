//
// Created by Ricardo on 29/03/2024.
//

#include <duk_renderer/font/font_handler.h>
#include <duk_renderer/font/freetype/freetype_font_loader.h>

namespace duk::renderer {

FontHandler::FontHandler()
    : HandlerT("fnt") {
    m_loaders.emplace_back(std::make_unique<FreetypeFontLoader>());
}

bool FontHandler::accepts(const std::string& extension) const {
    return std::ranges::any_of(m_loaders, [&extension](const auto& loader) {
        return loader->accepts(extension);
    });
}

std::shared_ptr<Font> FontHandler::load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) {
    for (auto& loader: m_loaders) {
        if (loader->accepts(data, size)) {
            return loader->load(data, size);
        }
    }
    throw std::runtime_error("failed to find font loader for memory data");
}

}// namespace duk::renderer
