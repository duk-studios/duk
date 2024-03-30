//
// Created by Ricardo on 29/03/2024.
//

#include <duk_renderer/font/font_importer.h>
#include <duk_renderer/font/freetype/freetype_font_loader.h>

namespace duk::renderer {

FontImporter::FontImporter(const FontImporterCreateInfo& fontImporterCreateInfo)
    : m_fontPool(fontImporterCreateInfo.fontPool) {
    m_loaders.emplace_back(std::make_unique<FreetypeFontLoader>());
}

const std::string& FontImporter::tag() const {
    static const std::string tag = "fnt";
    return tag;
}

void FontImporter::load(const resource::Id& id, const std::filesystem::path& path) {
    for (auto& loader : m_loaders) {
        if (loader->accepts(path)) {
            m_fontPool->insert(id, loader->load(path));
            return;
        }
    }
    throw std::runtime_error(fmt::format("failed to find font loader for {}", path.string()));
}

} // duk::renderer
