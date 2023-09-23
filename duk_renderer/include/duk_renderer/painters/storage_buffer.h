/// 23/09/2023
/// storage_buffer.h

#ifndef DUK_RENDERER_STORAGE_BUFFER_H
#define DUK_RENDERER_STORAGE_BUFFER_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/rhi.h>

#include <duk_tools/fixed_vector.h>

#include <cstdint>

namespace duk::renderer {

struct StorageBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
};

template<typename T, size_t N>
class StorageBuffer {
public:

    explicit StorageBuffer(const StorageBufferCreateInfo& storageBufferCreateInfo);

    DUK_NO_DISCARD operator duk::rhi::Descriptor() const;

    DUK_NO_DISCARD T& at(size_t index);

    DUK_NO_DISCARD const T& at(size_t index) const;

    DUK_NO_DISCARD size_t capacity() const;

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD T& next();

    void push_back(const T& value);

    void push_back(T&& value);

    void clear();

    void flush();

private:
    duk::tools::FixedVector<T, N> m_data;
    std::shared_ptr<duk::rhi::Buffer> m_buffer;
};

template<typename T, size_t N>
StorageBuffer<T, N>::StorageBuffer(const StorageBufferCreateInfo& storageBufferCreateInfo) {
    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.commandQueue = storageBufferCreateInfo.commandQueue;
    bufferCreateInfo.type = rhi::Buffer::Type::STORAGE;
    bufferCreateInfo.elementSize = sizeof(T);
    bufferCreateInfo.elementCount = N;
    bufferCreateInfo.updateFrequency = rhi::Buffer::UpdateFrequency::DYNAMIC;

    auto expectedStorageBuffer = storageBufferCreateInfo.rhi->create_buffer(bufferCreateInfo);

    if (!expectedStorageBuffer) {
        throw std::runtime_error("failed to create StorageBuffer<T, N>: " + expectedStorageBuffer.error().description());
    }

    m_buffer = std::move(expectedStorageBuffer.value());
    flush();
}

template<typename T, size_t N>
StorageBuffer<T, N>::operator duk::rhi::Descriptor() const {
    return duk::rhi::Descriptor::storage_buffer(m_buffer.get());
}

template<typename T, size_t N>
T& StorageBuffer<T, N>::at(size_t index) {
    return m_data.at(index);
}

template<typename T, size_t N>
const T& StorageBuffer<T, N>::at(size_t index) const {
    return m_data.at(index);
}

template<typename T, size_t N>
size_t StorageBuffer<T, N>::capacity() const {
    return N;
}

template<typename T, size_t N>
size_t StorageBuffer<T, N>::size() const {
    return m_data.size();
}

template<typename T, size_t N>
T& StorageBuffer<T, N>::next() {
    return m_data.template emplace_back();
}

template<typename T, size_t N>
void StorageBuffer<T, N>::push_back(const T& value) {
    m_data.push_back(value);
}

template<typename T, size_t N>
void StorageBuffer<T, N>::push_back(T&& value) {
    m_data.push_back(std::move(value));
}

template<typename T, size_t N>
void StorageBuffer<T, N>::clear() {
    m_data.clear();
}

template<typename T, size_t N>
void StorageBuffer<T, N>::flush() {
    if (size() > 0) {
        m_buffer->write(m_data.data(), sizeof(T) * size(), 0);
    }
    m_buffer->flush();
}

}

#endif // DUK_RENDERER_STORAGE_BUFFER_H

