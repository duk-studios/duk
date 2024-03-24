//
// Created by Ricardo on 24/03/2024.
//

#ifndef DUK_AUDIO_DECODE_H
#define DUK_AUDIO_DECODE_H

#include <duk_audio/audio_buffer.h>

namespace duk::audio {

void decode_wav(void* wavData, size_t wavSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

}

#endif //DUK_AUDIO_DECODE_H
