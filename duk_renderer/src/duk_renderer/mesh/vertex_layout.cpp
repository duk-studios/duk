//
// Created by Ricardo on 07/04/2023.
//

#include <duk_renderer/mesh/vertex_layout.h>

#include <numeric>

namespace duk::renderer {

/// VertexAttribute ///

size_t VertexAttribute::size_of(VertexAttribute::Format type) {
    size_t result;
    switch (type) {
        case Format::UNDEFINED: result = 0; break;
        case Format::UINT8:
        case Format::INT8: result = 1; break;
        case Format::UINT16:
        case Format::INT16: result = 2; break;
        case Format::UINT32:
        case Format::INT32:
        case Format::FLOAT32: result = 4; break;
        case Format::VEC2: result = 8; break;
        case Format::VEC3: result = 12; break;
        case Format::VEC4: result = 16; break;
    }
    return result;
}

/// VertexLayout ///

VertexLayout::VertexLayout() = default;

VertexLayout::VertexLayout(const std::initializer_list<VertexAttribute::Format>& attributes) :
    m_attributes(attributes) {

}

void VertexLayout::insert(VertexAttribute::Format format) {
    m_attributes.push_back(format);
}

void VertexLayout::insert(const std::initializer_list<VertexAttribute::Format>& attributes) {
    m_attributes.insert(m_attributes.end(), attributes);
}

size_t VertexLayout::size() const {
    return m_attributes.size();
}

size_t VertexLayout::byte_size() const {
    size_t size = 0;
    for (auto& attribute : m_attributes){
        size += VertexAttribute::size_of(attribute);
    }
    return size;
}

VertexAttribute::Format VertexLayout::format_at(uint32_t location) const {
    return m_attributes[location];
}

std::vector<VertexAttribute::Format>::iterator VertexLayout::begin() {
    return m_attributes.begin();
}

std::vector<VertexAttribute::Format>::iterator VertexLayout::end() {
    return m_attributes.end();
}

std::vector<VertexAttribute::Format>::const_iterator VertexLayout::begin() const {
    return m_attributes.cbegin();
}

std::vector<VertexAttribute::Format>::const_iterator VertexLayout::end() const {
    return m_attributes.cend();
}

}