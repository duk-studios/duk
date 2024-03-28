//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RHI_VERTEX_DATA_SOURCE_H
#define DUK_RHI_VERTEX_DATA_SOURCE_H

#include <duk_hash/data_source.h>
#include <duk_renderer/vertex_types.h>
#include <duk_rhi/index_types.h>
#include <duk_rhi/vertex_layout.h>

#include <cstring>

namespace duk::renderer {

class MeshDataSource : public duk::hash::DataSource {
public:
    DUK_NO_DISCARD virtual size_t vertex_byte_count() const = 0;

    DUK_NO_DISCARD virtual size_t vertex_count() const = 0;

    DUK_NO_DISCARD virtual size_t index_byte_count() const = 0;

    DUK_NO_DISCARD virtual size_t index_count() const = 0;

    DUK_NO_DISCARD virtual duk::rhi::IndexType index_type() const = 0;

    DUK_NO_DISCARD virtual duk::rhi::VertexLayout vertex_layout() const = 0;

    DUK_NO_DISCARD virtual const VertexAttributes& vertex_attributes() const = 0;

    DUK_NO_DISCARD virtual bool has_vertex_attribute(VertexAttributes::Type vertexAttribute) const = 0;

    virtual void read_vertices(void* dst, uint32_t count, uint32_t offset) const = 0;

    virtual void read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const = 0;

    virtual void read_indices(void* dst, uint32_t count, uint32_t offset) const = 0;
};

template<typename V, typename I>
class MeshDataSourceT : public MeshDataSource {
public:
    using VertexType = V;
    using IndexType = I;

public:
    MeshDataSourceT();

    template<typename InputIterator>
    void insert_vertices(InputIterator begin, InputIterator end);

    template<typename InputIterator>
    void insert_indices(InputIterator begin, InputIterator end);

    DUK_NO_DISCARD size_t vertex_byte_count() const override;

    DUK_NO_DISCARD size_t vertex_count() const override;

    DUK_NO_DISCARD size_t index_byte_count() const override;

    DUK_NO_DISCARD size_t index_count() const override;

    DUK_NO_DISCARD duk::rhi::IndexType index_type() const override;

    DUK_NO_DISCARD duk::rhi::VertexLayout vertex_layout() const override;

    DUK_NO_DISCARD const VertexAttributes& vertex_attributes() const override;

    DUK_NO_DISCARD bool has_vertex_attribute(VertexAttributes::Type vertexAttribute) const override;

    void read_vertices(void* dst, uint32_t count, uint32_t offset) const override;

    void read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const override;

    void read_indices(void* dst, uint32_t count, uint32_t offset) const override;

protected:
    DUK_NO_DISCARD hash::Hash calculate_hash() const override;

private:
    VertexAttributes m_vertexAttributes;
    std::vector<V> m_vertices;
    std::vector<I> m_indices;
};

template<typename V, typename I>
MeshDataSourceT<V, I>::MeshDataSourceT()
    : m_vertexAttributes(VertexAttributes::create<V>()) {
}

template<typename V, typename I>
template<typename InputIterator>
void MeshDataSourceT<V, I>::insert_vertices(InputIterator begin, InputIterator end) {
    m_vertices.insert(m_vertices.end(), begin, end);
}

template<typename V, typename I>
template<typename InputIterator>
void MeshDataSourceT<V, I>::insert_indices(InputIterator begin, InputIterator end) {
    m_indices.insert(m_indices.end(), begin, end);
}

template<typename V, typename I>
size_t MeshDataSourceT<V, I>::vertex_byte_count() const {
    return m_vertices.size() * sizeof(V);
}

template<typename V, typename I>
size_t MeshDataSourceT<V, I>::vertex_count() const {
    return m_vertices.size();
}

template<typename V, typename I>
size_t MeshDataSourceT<V, I>::index_byte_count() const {
    return m_indices.size() * sizeof(I);
}

template<typename V, typename I>
size_t MeshDataSourceT<V, I>::index_count() const {
    return m_indices.size();
}

template<typename V, typename I>
duk::rhi::IndexType MeshDataSourceT<V, I>::index_type() const {
    return rhi::index_type<I>();
}

template<typename V, typename I>
duk::rhi::VertexLayout MeshDataSourceT<V, I>::vertex_layout() const {
    return m_vertexAttributes.vertex_layout();
}

template<typename V, typename I>
const VertexAttributes& MeshDataSourceT<V, I>::vertex_attributes() const {
    return m_vertexAttributes;
}

template<typename V, typename I>
bool MeshDataSourceT<V, I>::has_vertex_attribute(VertexAttributes::Type vertexAttribute) const {
    return m_vertexAttributes.has_attribute(vertexAttribute);
}

template<typename V, typename I>
void MeshDataSourceT<V, I>::read_vertices(void* dst, uint32_t count, uint32_t offset) const {
    assert(vertex_count() >= offset + count);
    const auto* src = m_vertices.data() + offset;
    std::memcpy(dst, src, count * sizeof(V));
}

template<typename V, typename I>
void MeshDataSourceT<V, I>::read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const {
    assert(vertex_count() >= offset + count);
    const auto* src = (uint8_t*)m_vertices.data();
    const auto attributeSize = VertexAttributes::size_of(attribute);
    const auto attributeOffset = m_vertexAttributes.offset_of(attribute);
    const auto stride = sizeof(V);
    for (int i = 0; i < count; i++) {
        const auto currentVertexByteOffset = (i + offset) * stride;
        const auto* srcAttribute = src + currentVertexByteOffset + attributeOffset;
        std::memcpy((uint8_t*)dst + (attributeSize * i), srcAttribute, attributeSize);
    }
}

template<typename V, typename I>
void MeshDataSourceT<V, I>::read_indices(void* dst, uint32_t count, uint32_t offset) const {
    assert(index_count() >= offset + count);
    const auto* src = m_indices.data() + offset;
    std::memcpy(dst, src, count * sizeof(I));
}

template<typename V, typename I>
hash::Hash MeshDataSourceT<V, I>::calculate_hash() const {
    hash::Hash hash = 0;
    if (!m_vertices.empty()) {
        hash::hash_combine(hash, m_vertices.begin(), m_vertices.end());
    }
    if (!m_indices.empty()) {
        hash::hash_combine(hash, m_indices.data(), index_byte_count());
    }
    return hash;
}

}// namespace duk::renderer

#endif//DUK_RHI_VERTEX_DATA_SOURCE_H
