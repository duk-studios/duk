//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_UI_FONT_HANDLER_H
#define DUK_UI_FONT_HANDLER_H

#include <duk_ui/font/font.h>

#include <duk_resource/handler.h>

namespace duk::ui {

class FontLoader {
public:
    virtual ~FontLoader() = default;

    virtual bool accepts(const std::filesystem::path& extension) = 0;

    virtual bool accepts(const void* data, size_t size) = 0;

    virtual std::shared_ptr<Font> load(const void* data, size_t size) = 0;
};

class FontHandler : public duk::resource::HandlerT<Font> {
public:
    FontHandler();

    bool accepts(const std::string& extension) const override;

protected:
    std::shared_ptr<Font> load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) override;

private:
    std::vector<std::unique_ptr<FontLoader>> m_loaders;
};

}// namespace duk::ui

#endif//DUK_UI_FONT_HANDLER_H
