/// 23/09/2023
/// storage_buffer.h

#ifndef DUK_RENDERER_STORAGE_BUFFER_H
#define DUK_RENDERER_STORAGE_BUFFER_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/rhi.h>

#include <duk_tools/fixed_vector.h>

#include <cmath>
#include <cstdint>
#include <numbers>

namespace duk::renderer {

struct StorageBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    size_t initialSize;
};

template<typename T>
class StorageBuffer {
public:
    static_assert(std::is_trivially_destructible<T>::value, "StorageBuffers can only store trivially destructible types");

    explicit StorageBuffer(const StorageBufferCreateInfo& storageBufferCreateInfo);

    DUK_NO_DISCARD duk::rhi::Descriptor descriptor() const;

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
    std::shared_ptr<duk::rhi::Buffer> create_buffer(size_t elementCount);

    T* ptr_at(size_t index);

    const T* const_ptr_at(size_t index) const;

private:
    duk::rhi::RHI* m_rhi;
    duk::rhi::CommandQueue* m_commandQueue;
    size_t m_size;
    std::shared_ptr<duk::rhi::Buffer> m_buffer;
};

template<typename T>
std::shared_ptr<duk::rhi::Buffer> StorageBuffer<T>::create_buffer(size_t elementCount) {
    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.commandQueue = m_commandQueue;
    bufferCreateInfo.type = rhi::Buffer::Type::STORAGE;
    bufferCreateInfo.elementSize = sizeof(T);
    bufferCreateInfo.elementCount = elementCount;
    bufferCreateInfo.updateFrequency = rhi::Buffer::UpdateFrequency::DYNAMIC;

    return m_rhi->create_buffer(bufferCreateInfo);
}

template<typename T>
StorageBuffer<T>::StorageBuffer(const StorageBufferCreateInfo& storageBufferCreateInfo)
    : m_rhi(storageBufferCreateInfo.rhi)
    , m_commandQueue(storageBufferCreateInfo.commandQueue)
    , m_size(0) {
    m_buffer = create_buffer(storageBufferCreateInfo.initialSize);
    flush();
}

template<typename T>
duk::rhi::Descriptor StorageBuffer<T>::descriptor() const {
    return duk::rhi::Descriptor::storage_buffer(m_buffer.get());
}

template<typename T>
T& StorageBuffer<T>::at(size_t index) {
    return *ptr_at(index);
}

template<typename T>
const T& StorageBuffer<T>::at(size_t index) const {
    return *const_ptr_at(index);
}

template<typename T>
size_t StorageBuffer<T>::capacity() const {
    return m_buffer->element_count();
}

template<typename T>
size_t StorageBuffer<T>::size() const {
    return m_size;
}

template<typename T>
T& StorageBuffer<T>::next() {
    if (m_size >= m_buffer->element_count()) {
        // create a new buffer and copy everything
        auto buffer = create_buffer(std::ceil(m_size * std::numbers::phi_v<float>));
        buffer->copy_from(m_buffer.get(), m_buffer->byte_size(), 0, 0);
        m_buffer = buffer;
    }
    auto ptr = ptr_at(m_size++);
    ::new (ptr) T();
    return *ptr;
}

template<typename T>
void StorageBuffer<T>::push_back(const T& value) {
    next() = value;
}

template<typename T>
void StorageBuffer<T>::push_back(T&& value) {
    next() = std::move(value);
}

template<typename T>
void StorageBuffer<T>::clear() {
    m_size = 0;
}

template<typename T>
void StorageBuffer<T>::flush() {
    m_buffer->flush();
}

template<typename T>
T* StorageBuffer<T>::ptr_at(size_t index) {
    return reinterpret_cast<T*>(m_buffer->write_ptr(index * sizeof(T)));
}

template<typename T>
const T* StorageBuffer<T>::const_ptr_at(size_t index) const {
    return reinterpret_cast<const T*>(m_buffer->read_ptr(index * sizeof(T)));
}

}// namespace duk::renderer

#endif// DUK_RENDERER_STORAGE_BUFFER_H
