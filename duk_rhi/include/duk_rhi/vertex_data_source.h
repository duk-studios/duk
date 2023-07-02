//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RHI_VERTEX_DATA_SOURCE_H
#define DUK_RHI_VERTEX_DATA_SOURCE_H

#include <duk_rhi/data_source.h>
#include <duk_rhi/vertex_layout.h>
#include <duk_rhi/vertex_types.h>

#include <cstring>

namespace duk::rhi {

class VertexDataSource : public DataSource {
public:

    DUK_NO_DISCARD virtual size_t byte_count() const = 0;

    DUK_NO_DISCARD virtual size_t vertex_count() const = 0;

    DUK_NO_DISCARD virtual VertexLayout vertex_layout() const = 0;

    virtual void read(uint8_t* dst, size_t size, size_t offset) const = 0;

};

template<typename T>
class VertexDataSourceInterleaved : public VertexDataSource {
public:
    using VertexType = T;

public:
    std::vector<T>& vector();

    DUK_NO_DISCARD size_t byte_count() const override;

    DUK_NO_DISCARD size_t vertex_count() const override;

    DUK_NO_DISCARD VertexLayout vertex_layout() const override;

    void read(uint8_t* dst, size_t size, size_t offset) const override;

protected:

    DUK_NO_DISCARD hash::Hash calculate_hash() const override;

private:
    std::vector<T> m_vector;
};

template<typename T>
std::vector<T>& VertexDataSourceInterleaved<T>::vector() {
    return m_vector;
}

template<typename T>
size_t VertexDataSourceInterleaved<T>::byte_count() const {
    return m_vector.size() * sizeof(T);
}

template<typename T>
size_t VertexDataSourceInterleaved<T>::vertex_count() const {
    return m_vector.size();
}

template<typename T>
VertexLayout VertexDataSourceInterleaved<T>::vertex_layout() const {
    return layout_of<T>();
}

template<typename T>
void VertexDataSourceInterleaved<T>::read(uint8_t* dst, size_t size, size_t offset) const {
    assert(byte_count() >= offset + size);
    uint8_t* src = (uint8_t*)m_vector.data() + offset;
    std::memcpy(dst, src, size);
}

template<typename T>
hash::Hash VertexDataSourceInterleaved<T>::calculate_hash() const {
    hash::Hash hash = 0;
    hash::hash_combine(hash, m_vector.begin(), m_vector.end());
    return hash;
}

}

#endif //DUK_RHI_VERTEX_DATA_SOURCE_H
