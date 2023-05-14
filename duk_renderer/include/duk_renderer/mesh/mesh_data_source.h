//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RENDERER_MESH_DATA_SOURCE_H
#define DUK_RENDERER_MESH_DATA_SOURCE_H

#include <duk_renderer/data_source.h>
#include <duk_renderer/mesh/vertex_data_source.h>

namespace duk::renderer {

class MeshDataSource : public DataSource {
public:

    MeshDataSource();

    void set_vertex_data_source(VertexDataSource* vertexDataSource);

    DUK_NO_DISCARD VertexDataSource* get_vertex_data_source();

protected:
    DUK_NO_DISCARD Hash calculate_hash() const override;

private:
    VertexDataSource* m_vertexDataSource;


};

}

#endif //DUK_RENDERER_MESH_DATA_SOURCE_H
