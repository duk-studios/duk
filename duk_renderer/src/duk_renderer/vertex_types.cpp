/// 15/11/2023
/// vertex_types.cpp

#include <duk_renderer/vertex_types.h>
#include <stdexcept>

namespace duk::renderer {

duk::rhi::VertexInput::Format VertexAttributes::format_of(VertexAttributes::Type attributeType) {
    switch (attributeType) {
        case Type::POSITION:
            return duk::rhi::VertexInput::Format::VEC3;
        case Type::NORMAL:
            return duk::rhi::VertexInput::Format::VEC3;
        case Type::UV:
            return duk::rhi::VertexInput::Format::VEC2;
        case Type::COLOR:
            return duk::rhi::VertexInput::Format::VEC4;
        default:
            throw std::invalid_argument("unknown VertexInput::Format for VertexAttribute::Type");
    }
}

size_t VertexAttributes::size_of(VertexAttributes::Type attributeType) {
    return duk::rhi::VertexInput::size_of(format_of(attributeType));
}

VertexAttributes::VertexAttributes()
    : m_attributeOffset({}) {
}

VertexAttributes::VertexAttributes(const std::set<Type>& attributes) {
    size_t offset = 0;
    for (auto attributeIndex = 0; attributeIndex < COUNT; attributeIndex++) {
        const auto attributeType = static_cast<Type>(attributeIndex);
        const auto format = format_of(attributeType);
        if (!attributes.contains(attributeType)) {
            m_layout.insert(duk::rhi::VertexInput::Format::UNDEFINED);
            continue;
        }
        m_layout.insert(format);
        m_attributes.set(attributeIndex);
        m_attributeOffset[attributeIndex] = offset;
        offset += duk::rhi::VertexInput::size_of(format);
    }
}

bool VertexAttributes::has_attribute(VertexAttributes::Type attributeType) const {
    return m_attributes.test(static_cast<uint32_t>(attributeType));
}

const rhi::VertexLayout& VertexAttributes::vertex_layout() const {
    return m_layout;
}

size_t VertexAttributes::offset_of(VertexAttributes::Type attributeType) const {
    if (!has_attribute(attributeType)) {
        throw std::invalid_argument("VertexAttributes::Type is not present");
    }
    return m_attributeOffset[static_cast<uint32_t>(attributeType)];
}

VertexAttributes::ConstIterator VertexAttributes::begin() const {
    return m_attributes.bits<true>().begin();
}

VertexAttributes::ConstIterator VertexAttributes::end() const {
    return m_attributes.bits<true>().end();
}

}// namespace duk::renderer