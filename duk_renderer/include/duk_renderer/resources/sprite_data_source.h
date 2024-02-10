//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_DATA_SOURCE_H
#define DUK_RENDERER_SPRITE_DATA_SOURCE_H

#include <duk_resource/resource.h>
#include <duk_hash/data_source.h>

namespace duk::renderer {

struct SpriteDataSourceCreateInfo {
    duk::resource::Id imageId;
    glm::vec2 imageMin;
    glm::vec2 imageMax;
};

class SpriteDataSource : public duk::hash::DataSource {
public:

    explicit SpriteDataSource(const SpriteDataSourceCreateInfo& spriteDataSourceCreateInfo);

    DUK_NO_DISCARD duk::resource::Id image_id() const;

    DUK_NO_DISCARD glm::vec2 image_min() const;

    DUK_NO_DISCARD glm::vec2 image_max() const;

protected:

    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override;

private:
    duk::resource::Id m_imageId;
    glm::vec2 m_imageMin;
    glm::vec2 m_imageMax;
};

}

#endif //DUK_RENDERER_SPRITE_DATA_SOURCE_H
