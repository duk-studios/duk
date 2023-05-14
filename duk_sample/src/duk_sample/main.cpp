//
// Created by Ricardo on 09/04/2023.
//

#include <duk_renderer/mesh/mesh_data_source.h>

int main() {
    using namespace duk::renderer;

    VertexDataSourceInterleaved<Vertex2DColor> vertexDataSource;

    vertexDataSource.vector() = {
            {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f, -0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
    };

    auto layout = vertexDataSource.vertex_layout();

    MeshDataSource meshDataSource;

    meshDataSource.set_vertex_data_source(&vertexDataSource);

}