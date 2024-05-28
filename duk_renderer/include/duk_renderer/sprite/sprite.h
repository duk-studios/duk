//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_H
#define DUK_RENDERER_SPRITE_H

#include <duk_renderer/image/image_pool.h>

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

namespace duk::serial {

template<>
inline void from_json<duk::renderer::SpriteData>(const rapidjson::Value& json, duk::renderer::SpriteData& spriteData) {
    from_json_member(json, "max", spriteData.max);
    from_json_member(json, "min", spriteData.min);
    from_json_member(json, "pivot", spriteData.pivot);
}

template<>
inline void to_json<duk::renderer::SpriteData>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::SpriteData& spriteData) {
    to_json_member(document, json, "max", spriteData.max);
    to_json_member(document, json, "min", spriteData.min);
    to_json_member(document, json, "pivot", spriteData.pivot);
}

template<>
inline void from_json<duk::renderer::SpriteAtlasData>(const rapidjson::Value& json, duk::renderer::SpriteAtlasData& spriteAtlasData) {
    from_json_member(json, "image", spriteAtlasData.image);
    from_json_member(json, "pixelsPerUnit", spriteAtlasData.pixelsPerUnit);
    from_json_member(json, "sprites", spriteAtlasData.sprites);
}

template<>
inline void to_json<duk::renderer::SpriteAtlasData>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::SpriteAtlasData& spriteAtlasData) {
    to_json_member(document, json, "image", spriteAtlasData.image);
    to_json_member(document, json, "pixelsPerUnit", spriteAtlasData.pixelsPerUnit);
    to_json_member(document, json, "sprites", spriteAtlasData.sprites);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::Sprite& sprite) {
    solver->solve(sprite.image());
}

}// namespace duk::resource

#endif//DUK_RENDERER_SPRITE_H
