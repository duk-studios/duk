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
    OGG//not supported yet
};

void decode(Encoding encoding, const void* encodedData, size_t encodedSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode(const void* encodedData, size_t encodedSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_wav(const void* wavData, size_t wavSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_mp3(const void* mp3Data, size_t mp3Size, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_flac(const void* flacData, size_t flacSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

void decode_ogg(const void* oggData, size_t oggSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate);

}// namespace duk::audio

#endif//DUK_AUDIO_DECODE_H
