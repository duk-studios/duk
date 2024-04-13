//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_RENDERER_FONT_HANDLER_H
#define DUK_RENDERER_FONT_HANDLER_H

#include <duk_renderer/font/font_pool.h>

#include <duk_resource/handler.h>

namespace duk::renderer {

class FontLoader {
public:
    virtual ~FontLoader() = default;

    virtual bool accepts(const std::filesystem::path& path) = 0;

    virtual std::shared_ptr<Font> load(const std::filesystem::path& path) = 0;
};

struct FontHandlerCreateInfo {
    FontPool* fontPool;
    Renderer* renderer;
};

class FontHandler : public duk::resource::ResourceHandlerT<FontPool> {
public:
    FontHandler();

protected:
    void load(FontPool* pool, const resource::Id& id, const std::filesystem::path& path) override;

private:
    std::vector<std::unique_ptr<FontLoader>> m_loaders;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_FONT_HANDLER_H
