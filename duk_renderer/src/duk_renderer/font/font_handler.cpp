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

duk::resource::Handle<Font> FontHandler::load_from_memory(FontPool* pool, const resource::Id& id, const void* data, size_t size) {
    for (auto& loader: m_loaders) {
        if (loader->accepts(data, size)) {
            return pool->insert(id, loader->load(data, size));
        }
    }
    throw std::runtime_error(fmt::format("failed to find font loader for {}", id.value()));
}

}// namespace duk::renderer
