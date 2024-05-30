//
// Created by Ricardo on 28/05/2024.
//

#include <duk_renderer/mesh/mesh_builder.h>
#include <glm/ext/scalar_constants.hpp>

namespace duk::renderer {

namespace detail {

template<typename TIndex>
static bool calculate_normals(std::span<const glm::vec3> vertices, std::span<const TIndex> indices, std::span<glm::vec3> normals) {
    for (uint32_t i = 0; i < indices.size(); i += 3) {
        auto idx0 = indices[i];
        auto idx1 = indices[i + 1];
        auto idx2 = indices[i + 2];
        if (idx0 >= vertices.size() || idx1 >= vertices.size() || idx2 >= vertices.size()) {
            return false;
        }
        auto& v0 = vertices[idx0];
        auto& v1 = vertices[idx1];
        auto& v2 = vertices[idx2];

        auto& n0 = normals[idx0];
        auto& n1 = normals[idx1];
        auto& n2 = normals[idx2];

        n0 = n1 = n2 = glm::normalize(glm::cross(v0 - v1, v0 - v2));
    }
    return true;
}

static bool calculate_normals(std::span<const glm::vec3> vertices, std::span<glm::vec3> normals) {
    for (uint32_t i = 0; i < vertices.size(); i += 3) {
        auto idx0 = i;
        auto idx1 = i + 1;
        auto idx2 = i + 2;
        if (idx0 >= vertices.size() || idx1 >= vertices.size() || idx2 >= vertices.size()) {
            return false;
        }
        auto& v0 = vertices[idx0];
        auto& v1 = vertices[idx1];
        auto& v2 = vertices[idx2];

        auto& n0 = normals[idx0];
        auto& n1 = normals[idx1];
        auto& n2 = normals[idx2];

        n0 = n1 = n2 = glm::normalize(glm::cross(v0 - v1, v0 - v2));
    }
    return true;
}

}// namespace detail

bool calculate_normals(std::span<const glm::vec3> vertices, std::span<const uint32_t> indices, std::span<glm::vec3> normals) {
    return detail::calculate_normals(vertices, indices, normals);
}

bool calculate_normals(std::span<const glm::vec3> vertices, std::span<const uint16_t> indices, std::span<glm::vec3> normals) {
    return detail::calculate_normals(vertices, indices, normals);
}

bool calculate_normals(std::span<const glm::vec3> vertices, std::span<glm::vec3> normals) {
    return detail::calculate_normals(vertices, normals);
}

MeshDataSourceSOA build_quad_mesh_data(const std::set<VertexAttributes::Type>& attributes, const glm::vec3& scale) {
    MeshDataSourceSOA meshDataSource;
    if (attributes.contains(VertexAttributes::POSITION)) {
        std::array<glm::vec3, 4> positions = {};
        positions[0] = glm::vec3(-0.5f, -0.5f, 0.0f) * scale;
        positions[1] = glm::vec3(0.5f, -0.5f, 0.0f) * scale;
        positions[2] = glm::vec3(-0.5f, 0.5f, 0.0f) * scale;
        positions[3] = glm::vec3(0.5f, 0.5f, 0.0f) * scale;
        meshDataSource.insert_positions(positions.begin(), positions.end());
    }

    if (attributes.contains(VertexAttributes::NORMAL)) {
        std::array<glm::vec3, 4> normals = {};
        normals[0] = {0.0f, 0.0f, 1.0f};
        normals[1] = {0.0f, 0.0f, 1.0f};
        normals[2] = {0.0f, 0.0f, 1.0f};
        normals[3] = {0.0f, 0.0f, 1.0f};
        meshDataSource.insert_normals(normals.begin(), normals.end());
    }

    if (attributes.contains(VertexAttributes::UV)) {
        std::array<glm::vec2, 4> uvs = {};
        uvs[0] = {0.0f, 1.0f};
        uvs[1] = {1.0f, 1.0f};
        uvs[2] = {0.0f, 0.0f};
        uvs[3] = {1.0f, 0.0f};
        meshDataSource.insert_uvs(uvs.begin(), uvs.end());
    }

    if (attributes.contains(VertexAttributes::COLOR)) {
        std::array<glm::vec4, 4> colors = {};
        colors[0] = {1.0f, 1.0f, 1.0f, 1.0f};
        colors[1] = {1.0f, 1.0f, 1.0f, 1.0f};
        colors[2] = {1.0f, 1.0f, 1.0f, 1.0f};
        colors[3] = {1.0f, 1.0f, 1.0f, 1.0f};
        meshDataSource.insert_colors(colors.begin(), colors.end());
    }

    std::array<uint32_t, 6> indices = {0, 2, 1, 1, 2, 3};
    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

MeshDataSourceSOA build_cube_mesh_data(const std::set<VertexAttributes::Type>& attributes, const glm::vec3& scale) {
    MeshDataSourceSOA meshDataSource;

    if (attributes.contains(VertexAttributes::POSITION)) {
        std::array<glm::vec3, 24> positions = {};
        positions[0] = glm::vec3(-0.5f, -0.5f, 0.5f) * scale;
        positions[1] = glm::vec3(0.5f, -0.5f, 0.5f) * scale;
        positions[2] = glm::vec3(-0.5f, 0.5f, 0.5f) * scale;
        positions[3] = glm::vec3(0.5f, 0.5f, 0.5f) * scale;

        // back
        positions[4] = glm::vec3(0.5f, -0.5f, -0.5f) * scale;
        positions[5] = glm::vec3(-0.5f, -0.5f, -0.5f) * scale;
        positions[6] = glm::vec3(0.5f, 0.5f, -0.5f) * scale;
        positions[7] = glm::vec3(-0.5f, 0.5f, -0.5f) * scale;

        // left
        positions[8] = glm::vec3(-0.5f, -0.5f, -0.5f) * scale;
        positions[9] = glm::vec3(-0.5f, -0.5f, 0.5f) * scale;
        positions[10] = glm::vec3(-0.5f, 0.5f, -0.5f) * scale;
        positions[11] = glm::vec3(-0.5f, 0.5f, 0.5f) * scale;

        // right
        positions[12] = glm::vec3(0.5f, -0.5f, 0.5f) * scale;
        positions[13] = glm::vec3(0.5f, -0.5f, -0.5f) * scale;
        positions[14] = glm::vec3(0.5f, 0.5f, 0.5f) * scale;
        positions[15] = glm::vec3(0.5f, 0.5f, -0.5f) * scale;

        // top
        positions[16] = glm::vec3(-0.5f, 0.5f, 0.5f) * scale;
        positions[17] = glm::vec3(0.5f, 0.5f, 0.5f) * scale;
        positions[18] = glm::vec3(-0.5f, 0.5f, -0.5f) * scale;
        positions[19] = glm::vec3(0.5f, 0.5f, -0.5f) * scale;

        // bottom
        positions[20] = glm::vec3(-0.5f, -0.5f, -0.5f) * scale;
        positions[21] = glm::vec3(0.5f, -0.5f, -0.5f) * scale;
        positions[22] = glm::vec3(-0.5f, -0.5f, 0.5f) * scale;
        positions[23] = glm::vec3(0.5f, -0.5f, 0.5f) * scale;

        meshDataSource.insert_positions(positions.begin(), positions.end());
    }

    if (attributes.contains(VertexAttributes::NORMAL)) {
        std::array<glm::vec3, 24> normals = {};
        normals[0] = {0.0f, 0.0f, 1.0f};
        normals[1] = {0.0f, 0.0f, 1.0f};
        normals[2] = {0.0f, 0.0f, 1.0f};
        normals[3] = {0.0f, 0.0f, 1.0f};

        // back
        normals[4] = {0.0f, 0.0f, -1.0f};
        normals[5] = {0.0f, 0.0f, -1.0f};
        normals[6] = {0.0f, 0.0f, -1.0f};
        normals[7] = {0.0f, 0.0f, -1.0f};

        // left
        normals[8] = {-1.0f, 0.0f, 0.0f};
        normals[9] = {-1.0f, 0.0f, 0.0f};
        normals[10] = {-1.0f, 0.0f, 0.0f};
        normals[11] = {-1.0f, 0.0f, 0.0f};

        // right
        normals[12] = {1.0f, 0.0f, 0.0f};
        normals[13] = {1.0f, 0.0f, 0.0f};
        normals[14] = {1.0f, 0.0f, 0.0f};
        normals[15] = {1.0f, 0.0f, 0.0f};

        // top
        normals[16] = {0.0f, 1.0f, 0.0f};
        normals[17] = {0.0f, 1.0f, 0.0f};
        normals[18] = {0.0f, 1.0f, 0.0f};
        normals[19] = {0.0f, 1.0f, 0.0f};

        // bottom
        normals[20] = {0.0f, -1.0f, 0.0f};
        normals[21] = {0.0f, -1.0f, 0.0f};
        normals[22] = {0.0f, -1.0f, 0.0f};
        normals[23] = {0.0f, -1.0f, 0.0f};

        meshDataSource.insert_normals(normals.begin(), normals.end());
    }

    if (attributes.contains(VertexAttributes::UV)) {
        std::array<glm::vec2, 4> uvs = {};
        // front
        uvs[0] = {0.0f, 1.0f};
        uvs[1] = {1.0f, 1.0f};
        uvs[2] = {0.0f, 0.0f};
        uvs[3] = {1.0f, 0.0f};

        // once for each face
        for (auto i = 0; i < 6; i++) {
            meshDataSource.insert_uvs(uvs.begin(), uvs.end());
        }
        meshDataSource.insert_uvs(uvs.begin(), uvs.end());
    }

    if (attributes.contains(VertexAttributes::COLOR)) {
        constexpr glm::vec4 kWhite = {1.0f, 1.0f, 1.0f, 1.0f};
        std::array<glm::vec4, 24> colors;
        colors.fill(kWhite);
        meshDataSource.insert_colors(colors.begin(), colors.end());
    }

    std::array<uint32_t, 36> indices = {
            0,  2,  1,  1,  2,  3, // front
            4,  6,  5,  5,  6,  7, // back
            8,  10, 9,  9,  10, 11,// right
            12, 14, 13, 13, 14, 15,// left
            16, 18, 17, 17, 18, 19,// top
            20, 22, 21, 21, 22, 23 // bottom
    };

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

MeshDataSourceSOA build_sphere_mesh_data(uint32_t resolution, const std::set<VertexAttributes::Type>& attributes, const glm::vec3& scale) {
    MeshDataSourceSOA meshDataSource;

    const auto rows = resolution;
    const auto columns = resolution;

    std::vector<glm::vec3> positions;
    positions.reserve(rows * columns);
    std::vector<glm::vec3> normals;
    normals.reserve(rows * columns);
    std::vector<glm::vec2> uvs;
    uvs.reserve(rows * columns);
    std::vector<glm::vec4> colors;
    colors.reserve(rows * columns);

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

            if (attributes.contains(VertexAttributes::POSITION)) {
                positions.emplace_back(glm::vec3{x, y, z} * scale);
            }
            if (attributes.contains(VertexAttributes::NORMAL)) {
                normals.emplace_back(-glm::vec3(x, y, z));
            }
            if (attributes.contains(VertexAttributes::UV)) {
                uvs.emplace_back(u, v);
            }
            if (attributes.contains(VertexAttributes::COLOR)) {
                colors.emplace_back(glm::vec4(1));
            }
        }
    }

    meshDataSource.insert_positions(positions.begin(), positions.end());
    meshDataSource.insert_normals(normals.begin(), normals.end());
    meshDataSource.insert_uvs(uvs.begin(), uvs.end());
    meshDataSource.insert_colors(colors.begin(), colors.end());

    std::vector<uint32_t> indices;

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

MeshDataSourceSOA build_cone_mesh_data(uint32_t resolution, const std::set<VertexAttributes::Type>& attributes, const glm::vec3& scale) {
    MeshDataSourceSOA meshDataSource;

    uint32_t vertexCount = (resolution * 2) + 1;

    std::vector<glm::vec3> positions;
    positions.reserve(vertexCount);
    std::vector<glm::vec2> uvs;
    uvs.reserve(vertexCount);

    // create sides
    for (uint32_t i = 0; i < vertexCount; i++) {
        // start with pivot
        float percent = (float)i / (vertexCount - 1);
        if (i % 2) {
            positions.emplace_back(0, 1, 0);
            uvs.emplace_back(1.0f - percent, 0.0f);
        } else {
            float angle = percent * glm::pi<float>() * 2;
            float x = cosf(angle);
            float z = sinf(angle);

            positions.emplace_back(glm::vec3(x, 0, z) * 0.5f);
            uvs.emplace_back(glm::vec2(1.0f - percent, 1.0f));
        }
    }

    std::vector<uint32_t> indices;

    for (int i = 0; i < resolution * 2; i += 2) {
        uint32_t idx0 = i;
        uint32_t idx1 = i + 2;
        uint32_t idx2 = i + 1;

        indices.push_back(idx0);
        indices.push_back(idx1);
        indices.push_back(idx2);
    }

    // calculate normals for sides
    std::vector<glm::vec3> normals(vertexCount);
    calculate_normals(positions, indices, normals);

    uint32_t firstBaseVertexIndex = positions.size();
    // add base
    uint32_t baseVertexCount = resolution + 1;
    for (int i = 0; i < baseVertexCount; i++) {
        float percent = (float)i / (baseVertexCount - 1);
        float angle = percent * glm::pi<float>() * 2;
        float x = cosf(angle);
        float z = sinf(angle);

        positions.emplace_back(glm::vec3(x, 0, z) * 0.5f);
        normals.emplace_back(0, -1, 0);
        auto uv = (glm::vec2(x, z) + glm::vec2(1.0f)) / 2.0f;
        uv.y = 1 - uv.y;
        uvs.emplace_back(uv);
    }

    for (int i = 0; i < resolution - 1; i++) {
        uint32_t idx0 = firstBaseVertexIndex;
        uint32_t idx1 = idx0 + i + 2;
        uint32_t idx2 = idx0 + i + 1;

        indices.push_back(idx0);
        indices.push_back(idx1);
        indices.push_back(idx2);
    }

    if (attributes.contains(VertexAttributes::POSITION)) {
        meshDataSource.insert_positions(positions.begin(), positions.end());
    }
    if (attributes.contains(VertexAttributes::NORMAL)) {
        meshDataSource.insert_normals(normals.begin(), normals.end());
    }
    if (attributes.contains(VertexAttributes::UV)) {
        meshDataSource.insert_uvs(uvs.begin(), uvs.end());
    }
    if (attributes.contains(VertexAttributes::COLOR)) {
        std::vector<glm::vec4> colors(positions.size(), glm::vec4(1));
        meshDataSource.insert_colors(colors.begin(), colors.end());
    }

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

}// namespace duk::renderer
