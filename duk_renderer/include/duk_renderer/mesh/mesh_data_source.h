//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RENDERER_MESH_DATA_SOURCE_H
#define DUK_RENDERER_MESH_DATA_SOURCE_H

#include <duk_renderer/data_source.h>
#include <duk_renderer/mesh/vertex_layout.h>

namespace duk::renderer {

class MeshDataSource : public DataSource {
public:

private:
    VertexLayout m_vertexLayout;

};

}

#endif //DUK_RENDERER_MESH_DATA_SOURCE_H
