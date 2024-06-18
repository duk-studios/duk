//
// Created by Ricardo on 23/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_BUFFER_H
#define DUK_AUDIO_AUDIO_BUFFER_H

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace duk::audio {

class AudioBuffer {
public:
    virtual ~AudioBuffer() = default;

    virtual uint32_t read_float(float* dst, uint32_t frameCount, uint32_t frameOffset, uint32_t dstChannelCount) const = 0;

    virtual bool empty() const = 0;

    virtual uint32_t sample_count() const = 0;

    virtual uint32_t frame_count() const = 0;

    virtual uint32_t channel_count() const = 0;
};

template<typename TSample>
class AudioBufferT : public AudioBuffer {
public:
    AudioBufferT();

    AudioBufferT(uint32_t frameCount, uint32_t channelCount);

    AudioBufferT(const TSample* samples, uint32_t frameCount, uint32_t channelCount);

    ~AudioBufferT() override;

    uint32_t read_float(float* dst, uint32_t frameCount, uint32_t frameOffset, uint32_t dstChannelCount) const override;

    void resize(uint32_t frameCount, uint32_t channelCount);

    void clear();

    TSample* data();

    TSample* data() const;

    bool empty() const override;

    uint32_t sample_count() const override;

    uint32_t frame_count() const override;

    uint32_t channel_count() const override;

private:
    TSample* m_data;
    uint32_t m_frameCount;
    uint32_t m_channelCount;
};

template<typename TSample>
AudioBufferT<TSample>::AudioBufferT()
    : AudioBufferT(0, 0) {
}

template<typename TSample>
AudioBufferT<TSample>::AudioBufferT(uint32_t frameCount, uint32_t channelCount)
    : AudioBufferT(nullptr, frameCount, channelCount) {
}

template<typename TSample>
AudioBufferT<TSample>::AudioBufferT(const TSample* samples, uint32_t frameCount, uint32_t channelCount)
    : m_data(nullptr)
    , m_frameCount(0)
    , m_channelCount(0) {
    if (frameCount && channelCount) {
        resize(frameCount, channelCount);
    }
    if (samples) {
        std::memcpy(m_data, samples, frameCount * channelCount * sizeof(TSample));
    }
}

template<typename TSample>
AudioBufferT<TSample>::~AudioBufferT() {
    clear();
}

template<typename TSample>
uint32_t AudioBufferT<TSample>::read_float(float* dst, uint32_t frameCount, uint32_t frameOffset, uint32_t dstChannelCount) const {
    uint32_t framesToRead = std::min(frameCount, m_frameCount - frameOffset);
    for (auto channel = 0; channel < dstChannelCount; ++channel) {
        for (auto frame = 0; frame < framesToRead; ++frame) {
            const auto srcFrameIndex = frameOffset + frame;
            const auto dstFrameIndex = frame;
            const auto dstSampleIndex = dstFrameIndex * dstChannelCount + channel;
            const auto srcSampleIndex = srcFrameIndex * m_channelCount + (channel % m_channelCount);
            dst[dstSampleIndex] = m_data[srcSampleIndex];
        }
    }
    return framesToRead;
}

template<typename TSample>
void AudioBufferT<TSample>::resize(uint32_t frameCount, uint32_t channelCount) {
    auto totalSize = frameCount * channelCount * sizeof(TSample);
    if (totalSize == 0) {
        clear();
        return;
    }
    auto data = (TSample*)realloc(m_data, totalSize);
    if (data) {
        m_data = data;
        m_frameCount = frameCount;
        m_channelCount = channelCount;
    }
}

template<typename TSample>
void AudioBufferT<TSample>::clear() {
    if (m_data) {
        free(m_data);
        m_data = nullptr;
    }
    m_channelCount = 0;
    m_frameCount = 0;
}

template<typename TSample>
TSample* AudioBufferT<TSample>::data() {
    return m_data;
}

template<typename TSample>
TSample* AudioBufferT<TSample>::data() const {
    return m_data;
}

template<typename TSample>
bool AudioBufferT<TSample>::empty() const {
    return sample_count() > 0;
}

template<typename TSample>
uint32_t AudioBufferT<TSample>::sample_count() const {
    return m_frameCount * m_channelCount;
}

template<typename TSample>
uint32_t AudioBufferT<TSample>::frame_count() const {
    return m_frameCount;
}

template<typename TSample>
uint32_t AudioBufferT<TSample>::channel_count() const {
    return m_channelCount;
}

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_BUFFER_H
