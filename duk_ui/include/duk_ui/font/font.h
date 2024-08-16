//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_UI_FONT_H
#define DUK_UI_FONT_H

#include <duk_ui/font/font_atlas.h>

#include <duk_resource/handle.h>

namespace duk::ui {

class Font {
public:
    virtual ~Font();

    struct BuildAtlasParams {
        duk::rhi::RHI* rhi;
        duk::rhi::CommandQueue* commandQueue;
        uint32_t fontSize;
    };

    virtual FontAtlas* atlas(const BuildAtlasParams& buildAtlasParams) = 0;
};

using FontResource = duk::resource::Handle<Font>;

}// namespace duk::ui

#endif//DUK_UI_FONT_H
