//
// Created by Ricardo on 24/03/2024.
//

#ifndef DUK_AUDIO_DECODE_H
#define DUK_AUDIO_DECODE_H

#include <duk_audio/audio_buffer.h>

namespace duk::audio {

enum class Encoding {
    UNKNOWN = 0,
    WAV,
    MP3,
    FLAC,
    OGG //not supported yet
};

void decode(Encoding encoding, void* encodedData, size_t encodedSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode(void* encodedData, size_t encodedSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_wav(void* wavData, size_t wavSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_mp3(void* mp3Data, size_t mp3Size, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_flac(void* flacData, size_t flacSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_ogg(void* oggData, size_t oggSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

}

#endif //DUK_AUDIO_DECODE_H
