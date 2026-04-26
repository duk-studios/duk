//
// Created by rov on 1/9/2024.
//

#include <duk_renderer/sprite/sprite.h>
#include <duk_macros/assert.h>

namespace duk::renderer {

Sprite::Sprite(const SpriteAtlasCreateInfo& spriteAtlasCreateInfo)
    : m_image(spriteAtlasCreateInfo.spriteAtlasData->image)
    , m_pixelsPerUnit(spriteAtlasCreateInfo.spriteAtlasData->pixelsPerUnit)
    , m_sprites(spriteAtlasCreateInfo.spriteAtlasData->sprites) {
    DUK_ASSERT(!m_sprites.empty() && "Sprite atlas must contain at least one sprite");
    DUK_ASSERT(m_pixelsPerUnit > 0.0f && "Pixels per unit must be greater than zero");
}

ImageResource& Sprite::image() {
    return m_image;
}

const ImageResource& Sprite::image() const {
    return m_image;
}

uint32_t Sprite::count() const {
    return m_sprites.size();
}

SpriteMetrics Sprite::compute_metrics(uint32_t spriteIndex) const {
    SpriteMetrics metrics = {};
    if (spriteIndex < m_sprites.size() && m_image.valid()) {
        const auto imageSize = glm::vec2(m_image->size());
        const auto& sprite = m_sprites[spriteIndex];

        const auto spriteMin = glm::vec2(sprite.min.x, sprite.min.y);
        const auto spriteMax = glm::vec2(sprite.max.x, sprite.max.y);
        const auto spriteSize = (spriteMax - spriteMin) / m_pixelsPerUnit;
        const auto minOffset = spriteSize * sprite.pivot;

        metrics.uv.min = spriteMin / imageSize;
        metrics.uv.max = spriteMax / imageSize;
        metrics.position.min = -minOffset;
        metrics.position.max = spriteSize - minOffset;
    } else {
        metrics.uv.min = {0.0f, 0.0f};
        metrics.uv.max = {1.0f, 1.0f};
        metrics.position.min = {-0.5f, -0.5f};
        metrics.position.max = {0.5f, 0.5f};
    }

    return metrics;
}

}// namespace duk::renderer
