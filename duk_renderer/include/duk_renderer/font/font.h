//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_RENDERER_FONT_H
#define DUK_RENDERER_FONT_H

#include <duk_renderer/font/font_atlas.h>

#include <duk_resource/resource.h>

namespace duk::renderer {

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

using FontResource = duk::resource::ResourceT<Font>;

}// namespace duk::renderer

#endif//DUK_RENDERER_FONT_H
