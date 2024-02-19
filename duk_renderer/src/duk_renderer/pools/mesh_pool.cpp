//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/brushes/mesh_data_source.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/builtin_resource_ids.h>

#include <glm/ext/scalar_constants.hpp>

#include <cmath>

namespace duk::renderer {

namespace detail {

using DefaultMeshDataSource = duk::renderer::MeshDataSourceT<duk::renderer::VertexNormalUV, uint16_t>;

static void calculate_normals(std::vector<DefaultMeshDataSource::VertexType>& vertices, const std::vector<DefaultMeshDataSource::IndexType>& indices) {

    for (uint32_t i = 0; i < indices.size(); i += 3) {
        auto idx0 = indices[i];
        auto idx1 = indices[i + 1];

        auto idx2 = indices[i + 2];
        auto& v0 = vertices[idx0];
        auto& v1 = vertices[idx1];
        auto& v2 = vertices[idx2];
        v0.normal = v1.normal = v2.normal = glm::normalize(glm::cross(v1.position - v0.position, glm::vec3(0, 1, 0)));
    }
}

static DefaultMeshDataSource quad_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 4> vertices = {};
    vertices[0] = {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[1] = {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};
    vertices[2] = {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[3] = {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::array<DefaultMeshDataSource::IndexType, 6> indices = {0, 2, 1, 1, 2, 3};

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

static DefaultMeshDataSource cube_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 24> vertices = {};

    // front
    vertices[0] = {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 1.0f}};
    vertices[1] = {{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f}};
    vertices[2] = {{-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f}};
    vertices[3] = {{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 0.0f}};

    // back
    vertices[4] = {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}};
    vertices[5] = {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}};
    vertices[6] = {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}};
    vertices[7] = {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}};

    // left
    vertices[8] =  {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[9] =  {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[10] = {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[11] = {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};

    // right
    vertices[12] = {{0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[13] = {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[14] = {{0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[15] = {{0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};

    // top
    vertices[16] = {{-0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[17] = {{ 0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[18] = {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[19] = {{ 0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}};

    // bottom
    vertices[20] = {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[21] = {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[22] = {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[23] = {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}};

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::array<DefaultMeshDataSource::IndexType, 36> indices = {
            0, 2, 1, 1, 2, 3, // front
            4, 6, 5, 5, 6, 7, // back
            8, 10, 9, 9, 10, 11, // right
            12, 14, 13, 13, 14, 15, // left
            16, 18, 17, 17, 18, 19, // top
            20, 22, 21, 21, 22, 23 // bottom
    };

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

static DefaultMeshDataSource sphere_mesh_data_source(uint32_t segments) {
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

static DefaultMeshDataSource cone_mesh_data_source(uint32_t segments) {
    DefaultMeshDataSource meshDataSource;

    uint32_t vertexCount = (segments * 2) + 1;

    std::vector<DefaultMeshDataSource::VertexType> vertices(vertexCount);

    // create sides
    for (uint32_t i = 0; i < vertexCount; i++) {
        auto& vertex = vertices[i];
        // start with pivot
        float percent = (float)i / (vertexCount - 1);
        if (i % 2) {
            vertex.position = glm::vec3(0, 1, 0);
            vertex.uv = glm::vec2(1.0f - percent, 0.0f);
        }
        else {
            float angle = percent * glm::pi<float>() * 2;
            float x = cosf(angle);
            float z = sinf(angle);

            vertex.position = glm::vec3(x, 0, z) * 0.5f;
            vertex.uv = glm::vec2(1.0f - percent, 1.0f);
        }
    }

    std::vector<DefaultMeshDataSource::IndexType> indices;

    for (int i = 0; i < segments * 2; i += 2) {
        uint32_t idx0 = i;
        uint32_t idx1 = i + 2;
        uint32_t idx2 = i + 1;

        indices.push_back(idx0);
        indices.push_back(idx1);
        indices.push_back(idx2);
    }

    // calculate normals for sides
    calculate_normals(vertices, indices);

    uint32_t firstBaseVertexIndex = vertices.size();
    // add base
    uint32_t baseVertexCount = segments + 1;
    for (int i = 0; i < baseVertexCount; i++) {
        float percent = (float)i / (baseVertexCount - 1);
        float angle = percent * glm::pi<float>() * 2;
        float x = cosf(angle);
        float z = sinf(angle);

        auto vertex = DefaultMeshDataSource::VertexType();
        vertex.position = glm::vec3(x, 0, z) * 0.5f;
        vertex.normal = glm::vec3(0, -1, 0);
        vertex.uv = (glm::vec2(x, z) + glm::vec2(1.0f)) / 2.0f;
        vertex.uv.y = 1 - vertex.uv.y;
        vertices.push_back(vertex);
    }

    for (int i = 0; i < segments - 1; i++) {
        uint32_t idx0 = firstBaseVertexIndex;
        uint32_t idx1 = idx0 + i + 2;
        uint32_t idx2 = idx0 + i + 1;

        indices.push_back(idx0);
        indices.push_back(idx1);
        indices.push_back(idx2);
    }


    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

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
        m_quad = create(kQuadMeshId, &quadDataSource);
    }

    {
        auto cubeDataSource = detail::cube_mesh_data_source();
        m_cube = create(kCubeMeshId, &cubeDataSource);
    }

    {
        auto sphereDataSource = detail::sphere_mesh_data_source(32);
        m_sphere = create(kSphereMeshId, &sphereDataSource);
    }

    {
        auto coneDataSource = detail::cone_mesh_data_source(32);
        m_cone = create(kConeMeshId, &coneDataSource);
    }
}

MeshPool::~MeshPool() {
    m_quad.reset();
    m_cube.reset();
    m_sphere.reset();
    m_cone.reset();
    clean();
    m_meshBufferPool.reset();
}

MeshResource MeshPool::create(duk::resource::Id resourceId, const duk::renderer::MeshDataSource* meshDataSource) {
    return insert(resourceId, m_meshBufferPool->create_mesh(meshDataSource));
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

} // duk::renderer