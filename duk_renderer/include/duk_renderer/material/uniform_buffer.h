/// 23/09/2023
/// uniform_buffer.h

#ifndef DUK_RENDERER_UNIFORM_BUFFER_H
#define DUK_RENDERER_UNIFORM_BUFFER_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

template<typename T>
struct UniformBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    T initialData;
};

template<typename T>
class UniformBuffer {
public:
    UniformBuffer();

    explicit UniformBuffer(const UniformBufferCreateInfo<T>& uniformBufferCreateInfo);

    void create(duk::rhi::RHI* rhi, duk::rhi::CommandQueue* commandQueue);

    DUK_NO_DISCARD duk::rhi::Descriptor descriptor() const;

    DUK_NO_DISCARD T& data();

    DUK_NO_DISCARD const T& data() const;

    DUK_NO_DISCARD T* operator->();

    DUK_NO_DISCARD const T* operator->() const;

    DUK_NO_DISCARD duk::rhi::Buffer* buffer() const;

    DUK_NO_DISCARD bool valid() const;

    void flush();

private:
    T m_data;
    std::shared_ptr<duk::rhi::Buffer> m_buffer;
};

template<typename T>
UniformBuffer<T>::UniformBuffer()
    : m_data({}) {
}

template<typename T>
UniformBuffer<T>::UniformBuffer(const UniformBufferCreateInfo<T>& uniformBufferCreateInfo)
    : m_data(uniformBufferCreateInfo.initialData) {
    create(uniformBufferCreateInfo.rhi, uniformBufferCreateInfo.commandQueue);
    flush();
}

template<typename T>
void UniformBuffer<T>::create(duk::rhi::RHI* rhi, duk::rhi::CommandQueue* commandQueue) {
    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.commandQueue = commandQueue;
    bufferCreateInfo.type = rhi::Buffer::Type::UNIFORM;
    bufferCreateInfo.elementSize = sizeof(T);
    bufferCreateInfo.elementCount = 1;
    bufferCreateInfo.updateFrequency = rhi::Buffer::UpdateFrequency::DYNAMIC;

    m_buffer = rhi->create_buffer(bufferCreateInfo);
}

template<typename T>
duk::rhi::Descriptor UniformBuffer<T>::descriptor() const {
    return duk::rhi::Descriptor::uniform_buffer(m_buffer.get());
}

template<typename T>
T& UniformBuffer<T>::data() {
    return m_data;
}

template<typename T>
const T& UniformBuffer<T>::data() const {
    return m_data;
}

template<typename T>
T* UniformBuffer<T>::operator->() {
    return &m_data;
}

template<typename T>
const T* UniformBuffer<T>::operator->() const {
    return &m_data;
}

template<typename T>
duk::rhi::Buffer* UniformBuffer<T>::buffer() const {
    return m_buffer.get();
}

template<typename T>
bool UniformBuffer<T>::valid() const {
    return m_buffer != nullptr;
}

template<typename T>
void UniformBuffer<T>::flush() {
    m_buffer->write(m_data);
    m_buffer->flush();
}

}// namespace duk::renderer

#endif// DUK_RENDERER_UNIFORM_BUFFER_H
