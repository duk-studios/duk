//
// Created by rov on 11/21/2023.
//

#ifndef DUK_RENDERER_MATERIAL_POOL_H
#define DUK_RENDERER_MATERIAL_POOL_H

#include <duk_renderer/material/material.h>
#include <duk_resource/pool.h>

namespace duk::renderer {

class ImagePool;

struct MaterialPoolCreateInfo {
    Renderer* renderer;
};

class MaterialPool : public duk::resource::PoolT<MaterialResource> {
public:
    explicit MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo);

    MaterialResource create(duk::resource::Id resourceId, const MaterialData* materialData);

    MaterialResource create_color(const duk::resource::Pools& pools, duk::resource::Id resourceId);

    MaterialResource create_phong(const duk::resource::Pools& pools, duk::resource::Id resourceId);

private:
    duk::renderer::Renderer* m_renderer;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_MATERIAL_POOL_H
