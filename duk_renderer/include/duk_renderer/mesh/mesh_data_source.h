//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RHI_VERTEX_DATA_SOURCE_H
#define DUK_RHI_VERTEX_DATA_SOURCE_H

#include <duk_renderer/vertex_types.h>
#include <duk_hash/data_source.h>
#include <duk_rhi/index_types.h>

#include <cstring>

namespace duk::renderer {

class MeshDataSource : public duk::hash::DataSource {
public:
    DUK_NO_DISCARD virtual size_t vertex_count() const = 0;

    DUK_NO_DISCARD virtual size_t index_count() const = 0;

    DUK_NO_DISCARD virtual duk::rhi::IndexType index_type() const = 0;

    DUK_NO_DISCARD virtual VertexAttributes vertex_attributes() const = 0;

    DUK_NO_DISCARD virtual bool has_vertex_attribute(VertexAttributes::Type vertexAttribute) const = 0;

    virtual void read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const = 0;

    virtual void read_indices(void* dst, uint32_t count, uint32_t offset) const = 0;
};

class MeshDataSourceSOA : public MeshDataSource {
public:
    template<typename InputIterator>
    void insert_vertices(VertexAttributes::Type attribute, InputIterator begin, InputIterator end);

    template<typename InputIterator>
    void insert_positions(InputIterator begin, InputIterator end);

    template<typename InputIterator>
    void insert_normals(InputIterator begin, InputIterator end);

    template<typename InputIterator>
    void insert_uvs(InputIterator begin, InputIterator end);

    template<typename InputIterator>
    void insert_colors(InputIterator begin, InputIterator end);

    template<typename InputIterator>
    void insert_indices(InputIterator begin, InputIterator end);

    template<typename AttributeType>
    AttributeType* begin(VertexAttributes::Type attribute);

    template<typename AttributeType>
    AttributeType* end(VertexAttributes::Type attribute);

    DUK_NO_DISCARD size_t vertex_count() const override;

    DUK_NO_DISCARD size_t index_count() const override;

    DUK_NO_DISCARD duk::rhi::IndexType index_type() const override;

    DUK_NO_DISCARD VertexAttributes vertex_attributes() const override;

    DUK_NO_DISCARD bool has_vertex_attribute(VertexAttributes::Type vertexAttribute) const override;

    void read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const override;

    void read_indices(void* dst, uint32_t count, uint32_t offset) const override;

protected:
    DUK_NO_DISCARD hash::Hash calculate_hash() const override;

private:
    // a data buffer for each attribute, it doesn't mean that all of them are used
    std::array<std::vector<uint8_t>, VertexAttributes::COUNT> m_vertices;
    std::vector<uint32_t> m_indices;
};

template<typename V, typename I>
class MeshDataSourceAOS : public MeshDataSource {
public:
    using VertexType = V;
    using IndexType = I;

public:
    MeshDataSourceAOS();

    template<typename InputIterator>
    void insert_vertices(InputIterator begin, InputIterator end);

    template<typename InputIterator>
    void insert_indices(InputIterator begin, InputIterator end);

    DUK_NO_DISCARD size_t vertex_count() const override;

    DUK_NO_DISCARD size_t index_count() const override;

    DUK_NO_DISCARD duk::rhi::IndexType index_type() const override;

    DUK_NO_DISCARD VertexAttributes vertex_attributes() const override;

    DUK_NO_DISCARD bool has_vertex_attribute(VertexAttributes::Type vertexAttribute) const override;

    void read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const override;

    void read_indices(void* dst, uint32_t count, uint32_t offset) const override;

protected:
    DUK_NO_DISCARD hash::Hash calculate_hash() const override;

private:
    VertexAttributes m_vertexAttributes;
    std::vector<V> m_vertices;
    std::vector<I> m_indices;
};

template<typename InputIterator>
void MeshDataSourceSOA::insert_vertices(VertexAttributes::Type attribute, InputIterator begin, InputIterator end) {
    auto& attributeBuffer = m_vertices[attribute];
    const auto count = std::distance(begin, end);
    if (count == 0) {
        return;
    }
    const auto stride = VertexAttributes::size_of(attribute);
    const auto start = attributeBuffer.size();
    attributeBuffer.resize(start + count * stride);

    std::memcpy(attributeBuffer.data() + start, &*begin, count * stride);
}

template<typename InputIterator>
void MeshDataSourceSOA::insert_positions(InputIterator begin, InputIterator end) {
    insert_vertices(VertexAttributes::POSITION, begin, end);
}

template<typename InputIterator>
void MeshDataSourceSOA::insert_normals(InputIterator begin, InputIterator end) {
    insert_vertices(VertexAttributes::NORMAL, begin, end);
}

template<typename InputIterator>
void MeshDataSourceSOA::insert_uvs(InputIterator begin, InputIterator end) {
    insert_vertices(VertexAttributes::UV, begin, end);
}

template<typename InputIterator>
void MeshDataSourceSOA::insert_colors(InputIterator begin, InputIterator end) {
    insert_vertices(VertexAttributes::COLOR, begin, end);
}

template<typename InputIterator>
void MeshDataSourceSOA::insert_indices(InputIterator begin, InputIterator end) {
    m_indices.insert(m_indices.end(), begin, end);
}

template<typename AttributeType>
AttributeType* MeshDataSourceSOA::begin(VertexAttributes::Type attribute) {
    return static_cast<AttributeType*>(*m_vertices[attribute].begin());
}

template<typename AttributeType>
AttributeType* MeshDataSourceSOA::end(VertexAttributes::Type attribute) {
    auto& attributeBuffer = m_vertices[attribute];
    return static_cast<AttributeType*>(attributeBuffer.data() + attributeBuffer.size());
}

template<typename V, typename I>
MeshDataSourceAOS<V, I>::MeshDataSourceAOS()
    : m_vertexAttributes(VertexAttributes::create<V>()) {
}

template<typename V, typename I>
template<typename InputIterator>
void MeshDataSourceAOS<V, I>::insert_vertices(InputIterator begin, InputIterator end) {
    m_vertices.insert(m_vertices.end(), begin, end);
}

template<typename V, typename I>
template<typename InputIterator>
void MeshDataSourceAOS<V, I>::insert_indices(InputIterator begin, InputIterator end) {
    m_indices.insert(m_indices.end(), begin, end);
}

template<typename V, typename I>
size_t MeshDataSourceAOS<V, I>::vertex_count() const {
    return m_vertices.size();
}

template<typename V, typename I>
size_t MeshDataSourceAOS<V, I>::index_count() const {
    return m_indices.size();
}

template<typename V, typename I>
duk::rhi::IndexType MeshDataSourceAOS<V, I>::index_type() const {
    return rhi::index_type<I>();
}

template<typename V, typename I>
VertexAttributes MeshDataSourceAOS<V, I>::vertex_attributes() const {
    return m_vertexAttributes;
}

template<typename V, typename I>
bool MeshDataSourceAOS<V, I>::has_vertex_attribute(VertexAttributes::Type vertexAttribute) const {
    return m_vertexAttributes.has_attribute(vertexAttribute);
}

template<typename V, typename I>
void MeshDataSourceAOS<V, I>::read_vertices_attribute(VertexAttributes::Type attribute, void* dst, uint32_t count, uint32_t offset) const {
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
void MeshDataSourceAOS<V, I>::read_indices(void* dst, uint32_t count, uint32_t offset) const {
    assert(index_count() >= offset + count);
    const auto* src = m_indices.data() + offset;
    std::memcpy(dst, src, count * sizeof(I));
}

template<typename V, typename I>
hash::Hash MeshDataSourceAOS<V, I>::calculate_hash() const {
    hash::Hash hash = 0;
    if (!m_vertices.empty()) {
        hash::hash_combine(hash, m_vertices.begin(), m_vertices.end());
    }
    if (!m_indices.empty()) {
        hash::hash_combine(hash, m_indices.data(), m_indices.size() * sizeof(I));
    }
    return hash;
}

}// namespace duk::renderer

#endif//DUK_RHI_VERTEX_DATA_SOURCE_H
