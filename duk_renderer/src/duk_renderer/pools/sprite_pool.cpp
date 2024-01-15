//
// Created by rov on 1/9/2024.
//
#include <duk_renderer/pools/sprite_pool.h>

namespace duk::renderer {

SpritePool::SpritePool(const SpritePoolCreateInfo& spritePoolCreateInfo) :
    m_imagePool(spritePoolCreateInfo.imagePool) {

}

SpriteResource SpritePool::create(duk::pool::ResourceId resourceId, const duk::renderer::SpriteDataSource* spriteDataSource) {
    SpriteCreateInfo spriteCreateInfo = {};
    spriteCreateInfo.spriteDataSource = spriteDataSource;
    spriteCreateInfo.imagePool = m_imagePool;
    return insert(resourceId, std::make_shared<Sprite>(spriteCreateInfo));
}

}