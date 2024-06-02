//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/mesh/static_mesh_data_source.h>
#include <duk_renderer/mesh/mesh_pool.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_renderer/mesh/static_mesh.h>
#include <duk_renderer/mesh/static_mesh_builder.h>
#include <duk_renderer/renderer.h>

namespace duk::renderer {

MeshPool::MeshPool(const MeshPoolCreateInfo& meshPoolCreateInfo)
    : m_meshBufferPool(meshPoolCreateInfo.meshBufferPool) {
    {
        auto quadDataSource = build_quad_mesh_data();
        m_quad = create(kQuadMeshId, &quadDataSource);
    }

    {
        auto cubeDataSource = build_cube_mesh_data();
        m_cube = create(kCubeMeshId, &cubeDataSource);
    }

    {
        auto sphereDataSource = build_sphere_mesh_data(32);
        m_sphere = create(kSphereMeshId, &sphereDataSource);
    }

    {
        auto coneDataSource = build_cone_mesh_data(32);
        m_cone = create(kConeMeshId, &coneDataSource);
    }
}

MeshPool::~MeshPool() = default;

MeshResource MeshPool::create(duk::resource::Id resourceId, const duk::renderer::StaticMeshDataSource* meshDataSource) {
    StaticMeshCreateInfo meshCreateInfo = {};
    meshCreateInfo.meshBufferPool = m_meshBufferPool;
    meshCreateInfo.meshDataSource = meshDataSource;
    return insert(resourceId, std::make_shared<StaticMesh>(meshCreateInfo));
}

MeshResource MeshPool::quad() const {
    return m_quad;
}

MeshResource MeshPool::cube() const {
    return m_cube;
}

MeshResource MeshPool::sphere() const {
    return m_sphere;
}

MeshResource MeshPool::cone() const {
    return m_cone;
}

}// namespace duk::renderer