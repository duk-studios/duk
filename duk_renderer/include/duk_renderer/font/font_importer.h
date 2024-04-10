//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_RENDERER_FONT_IMPORTER_H
#define DUK_RENDERER_FONT_IMPORTER_H

#include <duk_renderer/font/font_pool.h>

#include <duk_resource/importer.h>

namespace duk::renderer {

class FontLoader {
public:
    virtual ~FontLoader() = default;

    virtual bool accepts(const std::filesystem::path& path) = 0;

    virtual std::shared_ptr<Font> load(const std::filesystem::path& path) = 0;
};

struct FontImporterCreateInfo {
    FontPool* fontPool;
};

class FontImporter : public duk::resource::ResourceImporter {
public:
    explicit FontImporter(const FontImporterCreateInfo& fontImporterCreateInfo);

    const std::string& tag() const override;

    void load(const resource::Id& id, const std::filesystem::path& path) override;

private:
    FontPool* m_fontPool;
    std::vector<std::unique_ptr<FontLoader>> m_loaders;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_FONT_IMPORTER_H
