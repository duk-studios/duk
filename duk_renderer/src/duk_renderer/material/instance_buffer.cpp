//
// Created by rov on 1/19/2024.
//

#include <numbers>
#include <duk_renderer/material/instance_buffer.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<duk::rhi::Buffer> create_storage_buffer(duk::rhi::RHI* rhi, duk::rhi::CommandQueue* commandQueue, uint32_t size) {
    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.type = duk::rhi::Buffer::Type::STORAGE;
    bufferCreateInfo.elementCount = 1;
    bufferCreateInfo.elementSize = size;
    bufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    bufferCreateInfo.commandQueue = commandQueue;
    return rhi->create_buffer(bufferCreateInfo);
}

}// namespace detail

MaterialInstanceBuffer::MaterialInstanceBuffer()
    : m_description()
    , m_rhi(nullptr)
    , m_commandQueue(nullptr)
    , m_bufferInstanceCount(0) {
}

MaterialInstanceBuffer::MaterialInstanceBuffer(const MaterialInstanceBufferCreateInfo& instanceBufferCreateInfo)
    : m_description(std::move(instanceBufferCreateInfo.description))
    , m_rhi(instanceBufferCreateInfo.rhi)
    , m_commandQueue(instanceBufferCreateInfo.commandQueue)
    , m_bufferInstanceCount(0) {
    m_defaultInstanceData.resize(m_description.stride);
    if (instanceBufferCreateInfo.instanceCount > 0) {
        expand(instanceBufferCreateInfo.instanceCount);
    }
}

bool MaterialInstanceBuffer::valid() const {
    return m_rhi != nullptr && m_commandQueue != nullptr && m_buffer != nullptr;
}

duk::rhi::Descriptor MaterialInstanceBuffer::descriptor() const {
    return duk::rhi::Descriptor::storage_buffer(m_buffer.get());
}

uint32_t MaterialInstanceBuffer::size() const {
    return m_instanceData.size() / m_description.stride;
}

uint32_t MaterialInstanceBuffer::byte_size() const {
    return m_instanceData.size();
}

bool MaterialInstanceBuffer::expand(uint32_t instanceIndex) {
    if (m_instanceVersions.size() <= instanceIndex) {
        m_instanceVersions.resize(instanceIndex + 1, ~0);
        const auto size = m_instanceVersions.size() * m_description.stride;
        m_instanceData.resize(size);
        auto buffer = detail::create_storage_buffer(m_rhi, m_commandQueue, size);
        if (m_buffer) {
            buffer->copy_from(m_buffer.get(), m_buffer->byte_size(), 0, 0);
        }
        m_buffer = std::move(buffer);
        return true;
    }
    return false;
}

void MaterialInstanceBuffer::write(const void* data, size_t size) {
    DUK_ASSERT(size == m_defaultInstanceData.size());
    std::memcpy(m_defaultInstanceData.data(), data, size);
}

void MaterialInstanceBuffer::write(uint32_t memberIndex, const void* data, size_t size) {
    const auto& member = m_description.members[memberIndex];
    DUK_ASSERT(member.size == size);
    std::memcpy(m_defaultInstanceData.data() + member.offset, data, size);
}

void MaterialInstanceBuffer::write(const duk::objects::Id& id, const void* data, size_t size) {
    const auto instanceOffset = instance_offset(id);
    if (!is_valid(id)) {
        update(id);
    }
    std::memcpy(m_instanceData.data() + instanceOffset, data, size);
}

void MaterialInstanceBuffer::write(const duk::objects::Id& id, uint32_t member, const void* data, size_t size) {
    const auto instanceOffset = instance_offset(id);
    const auto memberOffset = m_description.members[member].offset;
    const auto offset = instanceOffset + memberOffset;
    if (!is_valid(id)) {
        update(id);
    }
    std::memcpy(m_instanceData.data() + offset, data, size);
}

void MaterialInstanceBuffer::push(const duk::objects::Id& id) {
    if (!is_valid(id)) {
        update(id);
    }
    const auto instanceSize = m_description.stride;
    const auto instanceDataOffset = instance_offset(id);
    const auto bufferOffset = m_bufferInstanceCount * instanceSize;
    m_buffer->write(m_instanceData.data() + instanceDataOffset, instanceSize, bufferOffset);
    ++m_bufferInstanceCount;
}

void MaterialInstanceBuffer::clear() {
    m_bufferInstanceCount = 0;
}

void MaterialInstanceBuffer::flush() {
    m_buffer->flush();
}

uint32_t MaterialInstanceBuffer::instance_offset(const duk::objects::Id& id) const {
    return id.index() * m_description.stride;
}

bool MaterialInstanceBuffer::is_valid(const duk::objects::Id& id) const {
    return m_instanceVersions.size() > id.index() && m_instanceVersions[id.index()] == id.version();
}

void MaterialInstanceBuffer::update(const duk::objects::Id& id) {
    const auto offset = instance_offset(id);
    std::memcpy(m_instanceData.data() + offset, m_defaultInstanceData.data(), m_defaultInstanceData.size());
    m_instanceVersions[id.index()] = id.version();
}

}// namespace duk::renderer
