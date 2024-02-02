//
// Created by rov on 1/9/2024.
//

#include <duk_renderer/resources/sprite.h>

namespace duk::renderer {

Sprite::Sprite(const SpriteCreateInfo& spriteCreateInfo) :
    m_imageMin(),
    m_imageMax(),
    m_size() {
    const auto dataSource = spriteCreateInfo.spriteDataSource;
    const auto imagePool = spriteCreateInfo.imagePool;

    m_image = imagePool->find_or_default(dataSource->image_id(), imagePool->white_image());
    m_imageMin = dataSource->image_min();
    m_imageMax = dataSource->image_max();

    const auto sizePercent = m_imageMax - m_imageMin;
    const auto width = static_cast<uint32_t>(static_cast<float>(m_image->width()) * sizePercent.x);
    const auto height = static_cast<uint32_t>(static_cast<float>(m_image->height()) * sizePercent.y);

    m_size = {width, height};
}

Sprite::~Sprite() = default;

ImageResource Sprite::image() const {
    return m_image;
}

glm::vec2 Sprite::image_min() const {
    return m_imageMin;
}

glm::vec2 Sprite::image_max() const {
    return m_imageMax;
}

glm::u32vec2 Sprite::size() const {
    return m_size;
}

uint32_t Sprite::width() const {
    return m_size.x;
}

uint32_t Sprite::height() const {
    return m_size.y;
}

}
