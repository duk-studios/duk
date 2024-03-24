//
// Created by Ricardo on 23/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_SOURCE_H
#define DUK_AUDIO_AUDIO_SOURCE_H

#include <cstdint>
#include <memory>

namespace duk::audio {

class AudioBuffer;

class AudioSource {
public:

    AudioSource(const std::shared_ptr<AudioBuffer>& buffer);

    uint32_t read_float(float* output, uint32_t frameCount, uint32_t channelCount);

    bool is_playing() const;

private:
    std::shared_ptr<AudioBuffer> m_buffer;
    uint32_t m_currentFrame;
};

}

#endif //DUK_AUDIO_AUDIO_SOURCE_H
