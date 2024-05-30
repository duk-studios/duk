//
// Created by Ricardo on 07/04/2023.
//

#include <duk_renderer/mesh/mesh_data_source.h>

namespace duk::renderer {

size_t MeshDataSourceSOA::vertex_count() const {
    return m_vertices[VertexAttributes::POSITION].size() / VertexAttributes::size_of(VertexAttributes::POSITION);
}

size_t MeshDataSourceSOA::index_count() const {
    return m_indices.size();
}

duk::rhi::IndexType MeshDataSourceSOA::index_type() const {
    return m_indices.empty() ? duk::rhi::IndexType::NONE : duk::rhi::IndexType::UINT32;
}

VertexAttributes MeshDataSourceSOA::vertex_attributes() const {
    std::set<VertexAttributes::Type> attributes;
    for (auto attributeIndex = 0; attributeIndex < VertexAttributes::COUNT; attributeIndex++) {
        auto attribute = static_cast<VertexAttributes::Type>(attributeIndex);
        auto& attributeBuffer = m_vertices[attribute];
        if (attributeBuffer.empty()) {
            continue;
        }
        attributes.insert(attribute);
    }
    return VertexAttributes(attributes);
}

bool MeshDataSourceSOA::has_vertex_attribute(VertexAttributes::Type vertexAttribute) const {
    return m_vertices[vertexAttribute].size() > 0;
}

void MeshDataSourceSOA::read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const {
    auto& attributeBuffer = m_vertices[attribute];
    auto stride = VertexAttributes::size_of(attribute);
    std::memcpy(dst, attributeBuffer.data() + offset * stride, count * stride);
}

void MeshDataSourceSOA::read_indices(void* dst, uint32_t count, uint32_t offset) const {
    std::memcpy(dst, m_indices.data() + offset, count * sizeof(uint32_t));
}

hash::Hash MeshDataSourceSOA::calculate_hash() const {
    hash::Hash hash = 0;
    for (auto& attribute: m_vertices) {
        if (!attribute.empty()) {
            hash::hash_combine(hash, attribute.data(), attribute.size());
        }
    }
    if (!m_indices.empty()) {
        hash::hash_combine(hash, m_indices.data(), m_indices.size() * sizeof(uint32_t));
    }
    return hash;
}

}// namespace duk::renderer