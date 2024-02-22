//
// Created by Ricardo on 07/04/2023.
//

#include <duk_rhi/vertex_layout.h>

#include <numeric>

namespace duk::rhi {

/// VertexInput ///

size_t VertexInput::size_of(VertexInput::Format type) {
    size_t result;
    switch (type) {
        case Format::UNDEFINED:
            result = 0;
            break;
        case Format::UINT8:
        case Format::INT8:
            result = 1;
            break;
        case Format::UINT16:
        case Format::INT16:
            result = 2;
            break;
        case Format::UINT32:
        case Format::INT32:
        case Format::FLOAT32:
            result = 4;
            break;
        case Format::VEC2:
            result = 8;
            break;
        case Format::VEC3:
            result = 12;
            break;
        case Format::VEC4:
            result = 16;
            break;
    }
    return result;
}

/// VertexLayout ///

VertexLayout::VertexLayout() = default;

VertexLayout::VertexLayout(const std::initializer_list<VertexInput::Format>& formats) : m_formats(formats) {
}

void VertexLayout::insert(VertexInput::Format format) {
    m_formats.push_back(format);
}

void VertexLayout::insert(const std::initializer_list<VertexInput::Format>& formats) {
    m_formats.insert(m_formats.end(), formats);
}

size_t VertexLayout::size() const {
    return m_formats.size();
}

size_t VertexLayout::byte_size() const {
    size_t size = 0;
    for (auto& attribute: m_formats) {
        size += VertexInput::size_of(attribute);
    }
    return size;
}

VertexInput::Format VertexLayout::format_at(uint32_t location) const {
    return m_formats[location];
}

std::vector<VertexInput::Format>::iterator VertexLayout::begin() {
    return m_formats.begin();
}

std::vector<VertexInput::Format>::iterator VertexLayout::end() {
    return m_formats.end();
}

std::vector<VertexInput::Format>::const_iterator VertexLayout::begin() const {
    return m_formats.cbegin();
}

std::vector<VertexInput::Format>::const_iterator VertexLayout::end() const {
    return m_formats.cend();
}

}// namespace duk::rhi