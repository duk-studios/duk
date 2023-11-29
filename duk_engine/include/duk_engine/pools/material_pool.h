//
// Created by rov on 11/21/2023.
//

#ifndef DUK_MATERIAL_POOL_H
#define DUK_MATERIAL_POOL_H

#include <duk_pool/pool.h>
#include <duk_engine/pools/image_pool.h>
#include <duk_renderer/materials/material.h>
#include <duk_renderer/materials/phong/phong_material.h>
#include <duk_renderer/materials/fullscreen/fullscreen_material.h>
#include <duk_renderer/materials/color/color_material.h>

namespace duk::engine {

struct MaterialPoolCreateInfo {
    duk::renderer::Renderer* renderer;
    ImagePool* imagePool;
};

class MaterialPool : public duk::pool::Pool<duk::renderer::Material> {
public:

    explicit MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo);

    DUK_NO_DISCARD duk::pool::Resource<duk::renderer::PhongMaterial> create_phong_material();

    DUK_NO_DISCARD duk::pool::Resource<duk::renderer::ColorMaterial> create_color_material();

    DUK_NO_DISCARD duk::pool::Resource<duk::renderer::FullscreenMaterial> create_fullscreen_material();

private:
    duk::renderer::Renderer* m_renderer;
    ImagePool* m_imagePool;
};

}

#endif //DUK_MATERIAL_POOL_H
