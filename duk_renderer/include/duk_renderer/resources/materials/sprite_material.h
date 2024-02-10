//
// Created by rov on 1/19/2024.
//

#ifndef DUK_SPRITE_MATERIAL_H
#define DUK_SPRITE_MATERIAL_H

#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/sprite.h>

namespace duk::renderer {

class SpriteMaterial : public Material {
protected:

    SpriteMaterial(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource);

public:

    struct PushSpriteParams {
        duk::scene::Object object;
        Sprite* sprite = nullptr;
    };

    DUK_NO_DISCARD virtual uint32_t get_sub_material_index(const SpriteMaterial::PushSpriteParams& params) = 0;

    virtual void apply(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params, uint32_t spriteHandle) = 0;

    virtual void clear_sub_materials() = 0;

private:
};

using SpriteMaterialResource = duk::resource::ResourceT<SpriteMaterial>;

}

namespace duk::resource {

template<>
struct BaseResource<duk::renderer::SpriteMaterial> {
    using Type = duk::renderer::Material;
};

}


#endif //DUK_SPRITE_MATERIAL_H
