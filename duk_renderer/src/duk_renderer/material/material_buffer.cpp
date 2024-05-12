//
// Created by Ricardo on 02/05/2024.
//

#include <duk_renderer/material/material_buffer.h>

namespace duk::renderer {

MaterialUniformBuffer::MaterialUniformBuffer() = default;

MaterialUniformBuffer::MaterialUniformBuffer(const MaterialBufferCreateInfo& materialBufferCreateInfo)
    : m_description(std::move(materialBufferCreateInfo.description)) {
    auto rhi = materialBufferCreateInfo.rhi;
    auto commandQueue = materialBufferCreateInfo.commandQueue;

    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.type = duk::rhi::Buffer::Type::UNIFORM;
    bufferCreateInfo.commandQueue = commandQueue;
    bufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    bufferCreateInfo.elementSize = m_description.stride;
    bufferCreateInfo.elementCount = 1;

    m_buffer = rhi->create_buffer(bufferCreateInfo);
}

bool MaterialUniformBuffer::valid() const {
    return m_buffer != nullptr;
}

duk::rhi::Descriptor MaterialUniformBuffer::descriptor() const {
    return duk::rhi::Descriptor::uniform_buffer(m_buffer.get());
}

void MaterialUniformBuffer::write(const void* data, uint32_t size) {
    DUK_ASSERT(size == m_description.stride);
    m_buffer->write(data, size, 0);
}

void MaterialUniformBuffer::write(uint32_t member, const void* data, uint32_t size) {
    const auto& memberDescription = m_description.members.at(member);
    DUK_ASSERT(size == memberDescription.size);
    m_buffer->write(data, size, memberDescription.offset);
}

}
