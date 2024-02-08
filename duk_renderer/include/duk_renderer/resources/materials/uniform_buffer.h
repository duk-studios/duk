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

    explicit UniformBuffer(const UniformBufferCreateInfo<T>& uniformBufferCreateInfo);

    DUK_NO_DISCARD operator duk::rhi::Descriptor() const;

    DUK_NO_DISCARD T& data();

    DUK_NO_DISCARD const T& data() const;

    DUK_NO_DISCARD duk::rhi::Buffer* buffer() const;

    void flush();

private:
    T m_data;
    std::shared_ptr<duk::rhi::Buffer> m_buffer;
};

template<typename T>
UniformBuffer<T>::UniformBuffer(const UniformBufferCreateInfo<T>& uniformBufferCreateInfo) :
    m_data(uniformBufferCreateInfo.initialData) {

    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.commandQueue = uniformBufferCreateInfo.commandQueue;
    bufferCreateInfo.type = rhi::Buffer::Type::UNIFORM;
    bufferCreateInfo.elementSize = sizeof(T);
    bufferCreateInfo.elementCount = 1;
    bufferCreateInfo.updateFrequency = rhi::Buffer::UpdateFrequency::DYNAMIC;

    m_buffer = uniformBufferCreateInfo.rhi->create_buffer(bufferCreateInfo);
    flush();
}

template<typename T>
UniformBuffer<T>::operator duk::rhi::Descriptor() const {
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
duk::rhi::Buffer* UniformBuffer<T>::buffer() const {
    return m_buffer.get();
}

template<typename T>
void UniformBuffer<T>::flush() {
    m_buffer->write(m_data);
    m_buffer->flush();
}

}

#endif // DUK_RENDERER_UNIFORM_BUFFER_H

