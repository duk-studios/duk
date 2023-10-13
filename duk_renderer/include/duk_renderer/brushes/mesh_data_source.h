//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RHI_VERTEX_DATA_SOURCE_H
#define DUK_RHI_VERTEX_DATA_SOURCE_H

#include <duk_renderer/vertex_types.h>
#include <duk_rhi/data_source.h>
#include <duk_rhi/vertex_layout.h>
#include <duk_rhi/index_types.h>

#include <cstring>

namespace duk::renderer {

class MeshDataSource : public duk::rhi::DataSource {
public:

    DUK_NO_DISCARD virtual size_t vertex_byte_count() const = 0;

    DUK_NO_DISCARD virtual size_t vertex_count() const = 0;

    DUK_NO_DISCARD virtual size_t index_byte_count() const = 0;

    DUK_NO_DISCARD virtual size_t index_count() const = 0;

    DUK_NO_DISCARD virtual duk::rhi::IndexType index_type() const = 0;

    DUK_NO_DISCARD virtual duk::rhi::VertexLayout vertex_layout() const = 0;

    virtual void read_vertices(uint8_t* dst, size_t size, size_t offset) const = 0;

    virtual void read_indices(uint8_t* dst, size_t size, size_t offset) const = 0;

};

template<typename V, typename I>
class MeshDataSourceT : public MeshDataSource {
public:
    using VertexType = V;
    using IndexType = I;

public:

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

    void read_vertices(uint8_t* dst, size_t size, size_t offset) const override;

    void read_indices(uint8_t* dst, size_t size, size_t offset) const override;

protected:

    DUK_NO_DISCARD hash::Hash calculate_hash() const override;

private:
    std::vector<V> m_vertices;
    std::vector<I> m_indices;
};

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
    return layout_of<V>();
}

template<typename V, typename I>
void MeshDataSourceT<V, I>::read_vertices(uint8_t* dst, size_t size, size_t offset) const {
    assert(vertex_byte_count() >= offset + size);
    uint8_t* src = (uint8_t*)m_vertices.data() + offset;
    std::memcpy(dst, src, size);
}

template<typename V, typename I>
void MeshDataSourceT<V, I>::read_indices(uint8_t* dst, size_t size, size_t offset) const {
    assert(index_byte_count() >= offset + size);
    uint8_t* src = (uint8_t*)m_indices.data() + offset;
    std::memcpy(dst, src, size);
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

}

#endif //DUK_RHI_VERTEX_DATA_SOURCE_H
