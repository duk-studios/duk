//
// Created by rov on 11/21/2023.
//

#ifndef DUK_RENDERER_MATERIAL_POOL_H
#define DUK_RENDERER_MATERIAL_POOL_H

#include <duk_resource/pool.h>
#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/materials/phong/phong_material.h>
#include <duk_renderer/resources/materials/fullscreen/fullscreen_material.h>
#include <duk_renderer/resources/materials/color/color_material.h>
#include <duk_renderer/resources/materials/sprites/sprite_color/sprite_color_material.h>

namespace duk::renderer {

class ImagePool;

struct MaterialPoolCreateInfo {
    Renderer* renderer;
};

class MaterialPool : public duk::resource::PoolT<MaterialResource> {
public:

    explicit MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo);

    MaterialResource create(duk::resource::Id resourceId, const MaterialDataSource* materialDataSource);

private:
    duk::renderer::Renderer* m_renderer;
};

}

#endif //DUK_RENDERER_MATERIAL_POOL_H
