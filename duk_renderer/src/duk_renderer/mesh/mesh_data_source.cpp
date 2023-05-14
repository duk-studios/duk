//
// Created by Ricardo on 07/04/2023.
//

#include <stdexcept>
#include "duk_renderer/mesh/mesh_data_source.h"

namespace duk::renderer {

MeshDataSource::MeshDataSource() :
    m_vertexDataSource(nullptr) {

}

void MeshDataSource::set_vertex_data_source(VertexDataSource* vertexDataSource) {
    m_vertexDataSource = vertexDataSource;
}

VertexDataSource* MeshDataSource::get_vertex_data_source() {
    return m_vertexDataSource;
}

Hash MeshDataSource::calculate_hash() const {
    Hash hash = 0;
    hash_combine(hash, m_vertexDataSource->hash());
    return hash;
}

}