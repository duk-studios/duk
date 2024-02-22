//
// Created by rov on 1/9/2024.
//
#include <duk_renderer/pools/sprite_pool.h>

namespace duk::renderer {

SpritePool::SpritePool(const SpritePoolCreateInfo& spritePoolCreateInfo) : m_imagePool(spritePoolCreateInfo.imagePool) {
    {
        SpriteDataSourceCreateInfo spriteDataSourceCreateInfo = {};
        spriteDataSourceCreateInfo.imageId = m_imagePool->white_image().id();
        spriteDataSourceCreateInfo.imageMin = glm::vec2(0);
        spriteDataSourceCreateInfo.imageMax = glm::vec2(1);

        SpriteDataSource spriteDataSource(spriteDataSourceCreateInfo);

        m_whiteSquareSprite = create(kWhiteSquareSpriteId, &spriteDataSource);
    }
}

SpriteResource SpritePool::create(duk::resource::Id resourceId, const duk::renderer::SpriteDataSource* spriteDataSource) {
    SpriteCreateInfo spriteCreateInfo = {};
    spriteCreateInfo.spriteDataSource = spriteDataSource;
    spriteCreateInfo.imagePool = m_imagePool;
    return insert(resourceId, std::make_shared<Sprite>(spriteCreateInfo));
}

SpriteResource SpritePool::white_square() const {
    return m_whiteSquareSprite;
}

}// namespace duk::renderer