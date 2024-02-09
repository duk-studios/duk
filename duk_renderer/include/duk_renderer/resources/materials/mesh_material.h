//
// Created by rov on 1/19/2024.
//

#ifndef DUK_RENDERER_MESH_MATERIAL_H
#define DUK_RENDERER_MESH_MATERIAL_H

#include <duk_renderer/resources/materials/material.h>

namespace duk::renderer {

class MeshMaterial : public Material {
public:

    using Material::Material;

    virtual void clear_instances();

    struct InsertInstanceParams {
        duk::scene::Object object;
    };

    virtual void insert_instance(const InsertInstanceParams& params);

    virtual void flush_instances();

    virtual void apply(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params) = 0;
};

using MeshMaterialResource = duk::resource::ResourceT<MeshMaterial>;

}

namespace duk::resource {

template<>
struct BaseResource<duk::renderer::MeshMaterial> {
    using Type = duk::renderer::Material;
};

}

#endif //DUK_RENDERER_MESH_MATERIAL_H
