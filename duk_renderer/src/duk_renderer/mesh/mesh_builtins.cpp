//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/mesh/static_mesh_data_source.h>
#include <duk_renderer/mesh/mesh_builtins.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_renderer/mesh/static_mesh.h>
#include <duk_renderer/mesh/static_mesh_builder.h>
#include <duk_renderer/renderer.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<Mesh> create_mesh(MeshBufferPool* meshBufferPool, StaticMeshDataSource* dataSource) {
    StaticMeshCreateInfo meshCreateInfo = {};
    meshCreateInfo.meshBufferPool = meshBufferPool;
    meshCreateInfo.meshDataSource = dataSource;
    return std::make_shared<StaticMesh>(meshCreateInfo);
}

}// namespace detail

MeshBuiltins::MeshBuiltins(const MeshBuiltinsCreateInfo& meshBuiltinsCreateInfo) {
    auto pools = meshBuiltinsCreateInfo.pools;
    auto meshBufferPool = meshBuiltinsCreateInfo.meshBufferPool;
    {
        auto quadDataSource = build_quad_mesh_data();
        m_quad = pools->insert(kQuadMeshId, detail::create_mesh(meshBufferPool, &quadDataSource));
    }

    {
        auto cubeDataSource = build_cube_mesh_data();
        m_cube = pools->insert(kCubeMeshId, detail::create_mesh(meshBufferPool, &cubeDataSource));
    }

    {
        auto sphereDataSource = build_sphere_mesh_data(32);
        m_sphere = pools->insert(kSphereMeshId, detail::create_mesh(meshBufferPool, &sphereDataSource));
    }

    {
        auto coneDataSource = build_cone_mesh_data(32);
        m_cone = pools->insert(kConeMeshId, detail::create_mesh(meshBufferPool, &coneDataSource));
    }
}

MeshResource MeshBuiltins::quad() const {
    return m_quad;
}

MeshResource MeshBuiltins::cube() const {
    return m_cube;
}

MeshResource MeshBuiltins::sphere() const {
    return m_sphere;
}

MeshResource MeshBuiltins::cone() const {
    return m_cone;
}

}// namespace duk::renderer