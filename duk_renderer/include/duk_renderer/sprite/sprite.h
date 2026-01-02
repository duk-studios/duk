//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_H
#define DUK_RENDERER_SPRITE_H

#include <duk_renderer/image/image_builtins.h>

namespace duk::renderer {

struct SpriteMetrics {
    struct {
        glm::vec2 min;
        glm::vec2 max;
    } position;

    struct {
        glm::vec2 min;
        glm::vec2 max;
    } uv;
};

struct SpriteData {
    glm::ivec2 min;
    glm::ivec2 max;
    glm::vec2 pivot;
};

struct SpriteAtlasData {
    ImageResource image;
    float pixelsPerUnit;
    std::vector<SpriteData> sprites;
};

struct SpriteAtlasCreateInfo {
    const SpriteAtlasData* spriteAtlasData;
};

class Sprite {
public:
    explicit Sprite(const SpriteAtlasCreateInfo& spriteAtlasCreateInfo);

    ImageResource& image();

    const ImageResource& image() const;

    uint32_t count() const;

    SpriteMetrics compute_metrics(uint32_t spriteIndex) const;

private:
    ImageResource m_image;
    float m_pixelsPerUnit;
    std::vector<SpriteData> m_sprites;
};

using SpriteResource = duk::resource::Handle<Sprite>;

}// namespace duk::renderer

namespace duk::type {

template<>
struct Type<duk::renderer::SpriteData> : Class<duk::renderer::SpriteData,
    Member<"max", &duk::renderer::SpriteData::max>,
    Member<"min", &duk::renderer::SpriteData::min>,
    Member<"pivot", &duk::renderer::SpriteData::pivot>> {
};

template<>
struct Type<duk::renderer::SpriteAtlasData> : Class<duk::renderer::SpriteAtlasData,
    Member<"image", &duk::renderer::SpriteAtlasData::image>,
    Member<"pixelsPerUnit", &duk::renderer::SpriteAtlasData::pixelsPerUnit>,
    Member<"sprites", &duk::renderer::SpriteAtlasData::sprites>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::Sprite& sprite) {
    solver->solve(sprite.image());
}

}// namespace duk::resource

#endif//DUK_RENDERER_SPRITE_H
