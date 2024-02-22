//
// Created by rov on 1/9/2024.
//
#include <duk_renderer/resources/sprite_data_source.h>

namespace duk::renderer {

SpriteDataSource::SpriteDataSource(const SpriteDataSourceCreateInfo& spriteDataSourceCreateInfo) :
    m_imageId(spriteDataSourceCreateInfo.imageId),
    m_imageMin(spriteDataSourceCreateInfo.imageMin),
    m_imageMax(spriteDataSourceCreateInfo.imageMax) {

}

duk::resource::Id SpriteDataSource::image_id() const {
    return m_imageId;
}

glm::vec2 SpriteDataSource::image_min() const {
    return m_imageMin;
}

glm::vec2 SpriteDataSource::image_max() const {
    return m_imageMax;
}

hash::Hash SpriteDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, m_imageId);
    duk::hash::hash_combine(hash, m_imageMin);
    duk::hash::hash_combine(hash, m_imageMax);
    return hash;
}

}