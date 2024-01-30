//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_H
#define DUK_RENDERER_SPRITE_H

#include <duk_renderer/resources/sprite_data_source.h>
#include <duk_renderer/pools/image_pool.h>

namespace duk::renderer {

struct SpriteCreateInfo {
    const SpriteDataSource* spriteDataSource;
    ImagePool* imagePool;
};

class Sprite {
public:

    explicit Sprite(const SpriteCreateInfo& spriteCreateInfo);

    ~Sprite();

    DUK_NO_DISCARD ImageResource image() const;

    DUK_NO_DISCARD glm::vec2 image_min() const;

    DUK_NO_DISCARD glm::vec2 image_max() const;

    DUK_NO_DISCARD glm::u32vec2 size() const;

    DUK_NO_DISCARD uint32_t width() const;

    DUK_NO_DISCARD uint32_t height() const;

private:
    ImageResource m_image;
    glm::vec2 m_imageMin;
    glm::vec2 m_imageMax;
    glm::u32vec2 m_size;
};

using SpriteResource = duk::pool::Resource<Sprite>;

}

#endif //DUK_RENDERER_SPRITE_H
