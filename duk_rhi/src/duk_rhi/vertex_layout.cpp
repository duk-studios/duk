//
// Created by Ricardo on 07/04/2023.
//

#include <duk_rhi/vertex_layout.h>

#include <numeric>
#include <stdexcept>

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
        default:
            throw std::runtime_error("Invalid VertexInput format");
    }
    return result;
}

/// VertexLayout ///

VertexLayout::VertexLayout() {
    m_formats.fill(VertexInput::Format::UNDEFINED);
}

VertexLayout::VertexLayout(const std::initializer_list<VertexInput::Format>& formats) : VertexLayout() {
    if (formats.size() > kMaxVertexShaderInputs) {
        throw std::runtime_error("VertexLayout: too many attributes");
    }
    std::copy_n(formats.begin(), formats.size(), m_formats.begin());
}

void VertexLayout::clear() {
    m_formats.fill(VertexInput::Format::UNDEFINED);
}

void VertexLayout::set(uint32_t location, VertexInput::Format format) {
    m_formats[location] = format;
}

VertexInput::Format VertexLayout::get(uint32_t location) const {
    return m_formats[location];
}

VertexInput::Format VertexLayout::format_at(uint32_t location) const {
    return m_formats[location];
}

VertexLayout::Container::iterator VertexLayout::begin() {
    return m_formats.begin();
}

VertexLayout::Container::iterator VertexLayout::end() {
    return m_formats.end();
}

VertexLayout::Container::const_iterator VertexLayout::begin() const {
    return m_formats.cbegin();
}

VertexLayout::Container::const_iterator VertexLayout::end() const {
    return m_formats.cend();
}

}// namespace duk::rhi