//
// Created by rov on 11/21/2023.
//

#ifndef DUK_ENGINE_MESH_POOL_H
#define DUK_ENGINE_MESH_POOL_H

#include <duk_pool/pool.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/brushes/mesh.h>

namespace duk::engine {

struct MeshPoolCreateInfo {
    duk::renderer::Renderer* renderer;
};

class MeshPool : public duk::pool::Pool<duk::renderer::Mesh> {
public:

    explicit MeshPool(const MeshPoolCreateInfo& meshPoolCreateInfo);

    ~MeshPool() override;

    DUK_NO_DISCARD ResourceHandle create(const duk::renderer::MeshDataSource* meshDataSource);

    DUK_NO_DISCARD ResourceHandle load(const std::string& path);

    DUK_NO_DISCARD ResourceHandle quad() const;

    DUK_NO_DISCARD ResourceHandle cube() const;

    DUK_NO_DISCARD ResourceHandle sphere() const;

private:
    ResourceHandle m_quad;
    ResourceHandle m_cube;
    ResourceHandle m_sphere;
    std::shared_ptr<duk::renderer::MeshBufferPool> m_meshBufferPool;
};

} // duk::engine

#endif //DUK_ENGINE_MESH_POOL_H
