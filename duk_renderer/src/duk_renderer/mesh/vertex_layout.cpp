//
// Created by Ricardo on 07/04/2023.
//

#include "duk_renderer/mesh/vertex_layout.h"

namespace duk::renderer {

/// VertexAttribute ///

size_t VertexAttribute::format_size(VertexAttribute::Format format) {
    size_t result;
    switch (format) {
        case Format::UNDEFINED: result = 0; break;
        case Format::UINT8:
        case Format::INT8: result = 1; break;
        case Format::UINT16:
        case Format::INT16: result = 2; break;
        case Format::UINT32:
        case Format::INT32:
        case Format::FLOAT: result = 4; break;
        case Format::VEC2: result = 8; break;
        case Format::VEC3: result = 12; break;
        case Format::VEC4: result = 16; break;
    }
    return result;
}

/// VertexLayout ///

void VertexLayout::enable(VertexAttribute::Type type, VertexAttribute::Format format) {
    m_attributes[type] = format;
}

void VertexLayout::disable(VertexAttribute::Type type) {
    m_attributes[type] = VertexAttribute::Format::UNDEFINED;
}

VertexLayout::VertexLayout() = default;

VertexLayout::VertexLayout(std::initializer_list<std::pair<VertexAttribute::Type, VertexAttribute::Format>> attributes) {
    for (auto[type, format] : attributes){
        enable(type, format);
    }
}

bool VertexLayout::is_enabled(VertexAttribute::Type type) const {
    return m_attributes[type] != VertexAttribute::Format::UNDEFINED;
}

VertexAttribute::Format VertexLayout::format_of(VertexAttribute::Type type) const {
    return m_attributes[type];
}

}