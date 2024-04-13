//
// Created by Ricardo on 30/03/2024.
//

#ifndef DUK_RENDERER_FONT_ATLAS_H
#define DUK_RENDERER_FONT_ATLAS_H

#include <duk_renderer/image/image.h>

#include <duk_rhi/rhi.h>

#include <memory>
#include <unordered_map>

namespace duk::renderer {

struct Glyph {
    // metrics, already in final pixel size
    struct {
        glm::ivec2 size;
        glm::ivec2 bearing;
        int32_t advance;
    } metrics;

    char chr;

    glm::ivec2 min;
    glm::ivec2 max;

    // coordinates in the atlas
    glm::vec2 uvMin;
    glm::vec2 uvMax;
};

struct FontAtlasCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    std::vector<Glyph> glyphs;
    std::vector<uint8_t> bitmap;
    glm::ivec2 bitmapSize;
    uint32_t fontSize;
};

class FontAtlas {
public:
    explicit FontAtlas(const FontAtlasCreateInfo& fontAtlasCreateInfo);

    ~FontAtlas();

    ImageResource image() const;

    bool find_glyph(char chr, Glyph& glyph, uint32_t fontSize) const;

    void scale_for(Glyph& glyph, uint32_t fontSize) const;

private:
    uint32_t m_fontSize;
    ImageResource m_image;
    std::unordered_map<char, Glyph> m_glyphs;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_FONT_ATLAS_H
