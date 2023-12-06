//
// Created by rov on 11/21/2023.
//

#ifndef DUK_RENDERER_MATERIAL_POOL_H
#define DUK_RENDERER_MATERIAL_POOL_H

#include <duk_pool/pool.h>
#include <duk_renderer/materials/material.h>
#include <duk_renderer/materials/phong/phong_material.h>
#include <duk_renderer/materials/fullscreen/fullscreen_material.h>
#include <duk_renderer/materials/color/color_material.h>

namespace duk::renderer {

class ImagePool;

using MaterialResource = duk::pool::Resource<Material>;
using PhongMaterialResource = duk::pool::Resource<PhongMaterial>;
using ColorMaterialResource = duk::pool::Resource<ColorMaterial>;
using FullscreenMaterialResource = duk::pool::Resource<FullscreenMaterial>;

struct MaterialPoolCreateInfo {
    Renderer* renderer;
    ImagePool* imagePool;
};

class MaterialPool : public duk::pool::Pool<MaterialResource> {
public:

    explicit MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo);

    DUK_NO_DISCARD MaterialResource create(duk::pool::ResourceId resourceId, const MaterialDataSource* materialDataSource);

    DUK_NO_DISCARD PhongMaterialResource create_phong(duk::pool::ResourceId resourceId);

    DUK_NO_DISCARD PhongMaterialResource create_phong(duk::pool::ResourceId resourceId, const PhongMaterialDataSource* phongMaterialDataSource);

    DUK_NO_DISCARD ColorMaterialResource create_color(duk::pool::ResourceId resourceId);

    DUK_NO_DISCARD ColorMaterialResource create_color(duk::pool::ResourceId resourceId, const ColorMaterialDataSource* colorMaterialDataSource);

    DUK_NO_DISCARD FullscreenMaterialResource create_fullscreen(duk::pool::ResourceId resourceId);

private:
    duk::renderer::Renderer* m_renderer;
    ImagePool* m_imagePool;
};

}

#endif //DUK_RENDERER_MATERIAL_POOL_H
