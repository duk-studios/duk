/// 15/11/2023
/// vertex_types.cpp

#include <duk_renderer/vertex_types.h>
#include <stdexcept>

namespace duk::renderer {

duk::rhi::VertexInput::Format VertexAttributes::format_of(VertexAttributes::Type attributeType) {
    switch (attributeType) {
        case Type::POSITION: return duk::rhi::VertexInput::Format::VEC3;
        case Type::NORMAL: return duk::rhi::VertexInput::Format::VEC3;
        case Type::UV: return duk::rhi::VertexInput::Format::VEC2;
        case Type::COLOR: return duk::rhi::VertexInput::Format::VEC4;
        default: throw std::invalid_argument("unknown VertexInput::Format for VertexAttribute::Type");
    }
}

VertexAttributes::VertexAttributes() {

}

VertexAttributes::VertexAttributes(std::vector<VertexAttributes::Type>&& attributes) :
    m_attributes(std::move(attributes)) {
    for (auto attribute : m_attributes) {
        m_layout.insert(format_of(attribute));
    }
}

bool VertexAttributes::has_attribute(VertexAttributes::Type attributeType) const {
    for (auto attribute : m_attributes) {
        if (attribute == attributeType) {
            return true;
        }
    }
    return false;
}

const rhi::VertexLayout& VertexAttributes::vertex_layout() const {
    return m_layout;
}

size_t VertexAttributes::offset_of(VertexAttributes::Type attributeType) const {
    size_t offset = 0;
    for (auto attribute : m_attributes) {
        if (attribute == attributeType) {
            return offset;
        }
        offset += duk::rhi::VertexInput::size_of(format_of(attribute));
    }
    throw std::invalid_argument("VertexAttributes::Type is not present");
}

VertexAttributes::ConstIterator VertexAttributes::begin() const {
    return m_attributes.begin();
}

VertexAttributes::ConstIterator VertexAttributes::end() const {
    return m_attributes.end();
}

}