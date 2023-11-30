//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/brushes/mesh_data_source.h>
#include <duk_renderer/renderer.h>
#include <glm/ext/scalar_constants.hpp>
#include <cmath>

namespace duk::renderer {

namespace detail {

using DefaultMeshDataSource = duk::renderer::MeshDataSourceT<duk::renderer::VertexNormalUV, uint16_t>;

DefaultMeshDataSource quad_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 4> vertices = {};
    vertices[0] = {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::array<DefaultMeshDataSource::IndexType, 6> indices = {0, 1, 2, 2, 1, 3};

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

DefaultMeshDataSource cube_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 24> vertices = {};

    // front
    vertices[0] = {{ 0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{-0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{ 0.5f, -0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}};

    // back
    vertices[4] = {{-0.5f,  0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[5] = {{ 0.5f,  0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[6] = {{-0.5f, -0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[7] = {{ 0.5f, -0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};

    // left
    vertices[8] =  {{-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[9] =  {{-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[10] = {{-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[11] = {{-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};

    // right
    vertices[12] = {{0.5f,  0.5f,  0.5f},   {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[13] = {{0.5f,  0.5f, -0.5f},   {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[14] = {{0.5f, -0.5f,  0.5f},   {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[15] = {{0.5f, -0.5f, -0.5f},   {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};

    // top
    vertices[16] = {{ 0.5f, 0.5f,  0.5f},   {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[17] = {{-0.5f, 0.5f,  0.5f},   {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[18] = {{ 0.5f, 0.5f, -0.5f},   {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[19] = {{-0.5f, 0.5f, -0.5f},   {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};

    // bottom
    vertices[20] = {{ 0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[21] = {{-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[22] = {{ 0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[23] = {{-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}};

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::array<DefaultMeshDataSource::IndexType, 36> indices = {
            0, 1, 2, 2, 1, 3, // front
            4, 5, 6, 6, 5, 7, // back
            8, 9, 10, 10, 9, 11, // right
            12, 13, 14, 14, 13, 15, // left
            16, 17, 18, 18, 17, 19, // top
            20, 21, 22, 22, 21, 23 // bottom
    };

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

DefaultMeshDataSource sphere_mesh_data_source(uint32_t segments) {
    DefaultMeshDataSource meshDataSource;

    const auto rows = segments;
    const auto columns = segments;

    std::vector<DefaultMeshDataSource::VertexType> vertices;

    for (int row = 0; row <= rows; ++row) {
        float rowAngle = static_cast<float>(row) * glm::pi<float>() / static_cast<float>(rows);
        float rowSin = std::sin(rowAngle);
        float rowCos = std::cos(rowAngle);

        for (int column = 0; column <= columns; ++column) {
            float columnAngle = static_cast<float>(column) * 2.0f * glm::pi<float>() / static_cast<float>(columns);
            float columnSin = std::sin(columnAngle);
            float columnCos = std::cos(columnAngle);

            float x = rowSin * columnCos;
            float y = rowCos;
            float z = rowSin * columnSin;
            float u = 1 - (static_cast<float>(column) / static_cast<float>(columns));
            float v = static_cast<float>(row) / static_cast<float>(rows);

            glm::vec3 position(x, y, z);
            glm::vec3 normal(x, y, z);
            glm::vec2 uv(u, v);

            vertices.push_back({position, normal, uv});
        }
    }

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::vector<DefaultMeshDataSource::IndexType> indices;

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            int firstVertexIndex = column + (row * (columns + 1));
            int secondVertexIndex = firstVertexIndex + 1;
            int thirdVertexIndex = column + ((row + 1) * (columns + 1));
            int fourthVertexIndex = thirdVertexIndex + 1;

            indices.push_back(firstVertexIndex);
            indices.push_back(thirdVertexIndex);
            indices.push_back(secondVertexIndex);

            indices.push_back(thirdVertexIndex);
            indices.push_back(fourthVertexIndex);
            indices.push_back(secondVertexIndex);
        }
    }

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

}

MeshPool::MeshPool(const MeshPoolCreateInfo& meshPoolCreateInfo) {

    {
        auto renderer = meshPoolCreateInfo.renderer;
        duk::renderer::MeshBufferPoolCreateInfo meshBufferPoolCreateInfo = {};
        meshBufferPoolCreateInfo.rhi = renderer->rhi();
        meshBufferPoolCreateInfo.commandQueue = renderer->main_command_queue();

        m_meshBufferPool = std::make_shared<renderer::MeshBufferPool>(meshBufferPoolCreateInfo);
    }

    {
        auto quadDataSource = detail::quad_mesh_data_source();
        m_quad = create(&quadDataSource);
    }

    {
        auto cubeDataSource = detail::cube_mesh_data_source();
        m_cube = create(&cubeDataSource);
    }

    {
        auto sphereDataSource = detail::sphere_mesh_data_source(32);
        m_sphere = create(&sphereDataSource);
    }
}

MeshPool::~MeshPool() {
    m_quad.reset();
    m_cube.reset();
    m_sphere.reset();
    clean();
    m_meshBufferPool.reset();
}

MeshResource MeshPool::create(const duk::renderer::MeshDataSource* meshDataSource) {
    return insert(m_meshBufferPool->create_mesh(meshDataSource));
}

MeshResource MeshPool::load(const std::string& path) {
    assert(false && "not implemented");
    return {};
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

} // duk::renderer