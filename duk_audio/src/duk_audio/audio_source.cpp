//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/audio_source.h>
#include <duk_audio/audio_buffer.h>

namespace duk::audio {

AudioSource::AudioSource(const std::shared_ptr<AudioBuffer>& buffer)
    : m_buffer(buffer)
    , m_currentFrame(0) {

}

uint32_t AudioSource::read_float(float* output, uint32_t frameCount, uint32_t channelCount) {
    uint32_t framesToRead = std::min(frameCount, m_buffer->frame_count() - m_currentFrame);
    if (framesToRead > 0) {
        m_buffer->read_float(output, framesToRead, m_currentFrame, channelCount);
        m_currentFrame += framesToRead;
    }
    return framesToRead;
}

bool AudioSource::is_playing() const {
    return m_currentFrame < m_buffer->frame_count();
}

}
