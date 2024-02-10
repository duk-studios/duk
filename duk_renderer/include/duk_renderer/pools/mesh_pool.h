//
// Created by rov on 11/21/2023.
//

#ifndef DUK_RENDERER_MESH_POOL_H
#define DUK_RENDERER_MESH_POOL_H

#include <duk_resource/pool.h>
#include <duk_renderer/brushes/mesh.h>

namespace duk::renderer {

class Renderer;

using MeshResource = duk::resource::ResourceT<Mesh>;

struct MeshPoolCreateInfo {
    Renderer* renderer;
};

class MeshPool : public duk::resource::PoolT<MeshResource> {
public:

    explicit MeshPool(const MeshPoolCreateInfo& meshPoolCreateInfo);

    ~MeshPool() override;

    DUK_NO_DISCARD MeshResource create(duk::resource::Id resourceId, const MeshDataSource* meshDataSource);

    DUK_NO_DISCARD MeshResource quad() const;

    DUK_NO_DISCARD MeshResource cube() const;

    DUK_NO_DISCARD MeshResource sphere() const;

private:
    MeshResource m_quad;
    MeshResource m_cube;
    MeshResource m_sphere;
    std::shared_ptr<MeshBufferPool> m_meshBufferPool;
};

} // duk::renderer

#endif //DUK_RENDERER_MESH_POOL_H
