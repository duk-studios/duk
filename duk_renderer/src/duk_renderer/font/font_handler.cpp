//
// Created by Ricardo on 29/03/2024.
//

#include <duk_renderer/font/font_handler.h>
#include <duk_renderer/font/freetype/freetype_font_loader.h>

namespace duk::renderer {

FontHandler::FontHandler(const FontHandlerCreateInfo& fontHandlerCreateInfo)
    : m_fontPool(fontHandlerCreateInfo.fontPool) {
    {
        FreetypeFontLoaderCreateInfo freetypeFontLoaderCreateInfo = {};
        freetypeFontLoaderCreateInfo.renderer = fontHandlerCreateInfo.renderer;
        m_loaders.emplace_back(std::make_unique<FreetypeFontLoader>(freetypeFontLoaderCreateInfo));
    }
}

const std::string& FontHandler::tag() const {
    static const std::string tag = "fnt";
    return tag;
}

void FontHandler::load(const resource::Id& id, const std::filesystem::path& path) {
    for (auto& loader: m_loaders) {
        if (loader->accepts(path)) {
            m_fontPool->insert(id, loader->load(path));
            return;
        }
    }
    throw std::runtime_error(fmt::format("failed to find font loader for {}", path.string()));
}

}// namespace duk::renderer
