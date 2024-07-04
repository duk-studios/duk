//
// Created by Ricardo on 04/07/2024.
//

#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <cstdint>

namespace duk::audio {

class AudioProcessor {
public:
    virtual ~AudioProcessor() = default;

    virtual void process(void* output, uint32_t frameCount, uint32_t channelCount) = 0;
};

}

#endif //AUDIO_PROCESSOR_H
