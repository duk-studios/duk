//
// Created by rov on 11/21/2023.
//

#ifndef DUK_RENDERER_MESH_POOL_H
#define DUK_RENDERER_MESH_POOL_H

#include <duk_renderer/mesh/mesh.h>
#include <duk_resource/pool.h>

namespace duk::renderer {

// 10,001 - 20,000 - reserved for built-in mesh
static constexpr duk::resource::Id kQuadMeshId(10001);
static constexpr duk::resource::Id kCubeMeshId(10002);
static constexpr duk::resource::Id kSphereMeshId(10003);
static constexpr duk::resource::Id kConeMeshId(10004);

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

    DUK_NO_DISCARD MeshResource cone() const;

private:
    MeshResource m_quad;
    MeshResource m_cube;
    MeshResource m_sphere;
    MeshResource m_cone;
    std::shared_ptr<MeshBufferPool> m_meshBufferPool;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_MESH_POOL_H
