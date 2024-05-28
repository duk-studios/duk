//
// Created by Ricardo on 29/03/2024.
//

#include <duk_renderer/font/font_handler.h>
#include <duk_renderer/font/freetype/freetype_font_loader.h>

namespace duk::renderer {

FontHandler::FontHandler()
    : ResourceHandlerT("fnt") {
    m_loaders.emplace_back(std::make_unique<FreetypeFontLoader>());
}

bool FontHandler::accepts(const std::string& extension) const {
    return std::ranges::any_of(m_loaders, [&extension](const auto& loader) {
        return loader->accepts(extension);
    });
}

duk::resource::Handle<Font> FontHandler::load(FontPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    for (auto& loader: m_loaders) {
        auto extension = path.extension();
        if (loader->accepts(extension)) {
            return pool->insert(id, loader->load(path));
        }
    }
    throw std::runtime_error(fmt::format("failed to find font loader for {}", path.string()));
}

}// namespace duk::renderer
