//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/decode.h>

// use miniaudio decoding
#include <duk_audio/miniaudio/miniaudio_import.h>

#include <stdexcept>

namespace duk::audio {

namespace detail {

static ma_encoding_format convert_encoding(Encoding encoding) {
    switch (encoding) {
        case Encoding::UNKNOWN:
            return ma_encoding_format_unknown;
        case Encoding::WAV:
            return ma_encoding_format_wav;
        case Encoding::MP3:
            return ma_encoding_format_mp3;
        case Encoding::FLAC:
            return ma_encoding_format_flac;
        case Encoding::OGG:
            return ma_encoding_format_vorbis;
        default:
            throw std::invalid_argument("unsupported encoding type");
    }
}

}// namespace detail

void decode(Encoding encoding, const void* encodedData, size_t encodedSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate) {
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, channelCount, frameRate);
    decoderConfig.encodingFormat = detail::convert_encoding(encoding);

    ma_decoder decoder;
    ma_result result;
    result = ma_decoder_init_memory(encodedData, encodedSize, &decoderConfig, &decoder);
    if (result != MA_SUCCESS) {
        throw std::runtime_error("failed to initialize decoder");
    }

    uint64_t frameCount;
    result = ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount);
    if (result != MA_SUCCESS) {
        throw std::runtime_error("failed to get decoder length");
    }

    buffer->resize(frameCount, channelCount);

    result = ma_decoder_read_pcm_frames(&decoder, buffer->data(), frameCount, nullptr);
    if (result != MA_SUCCESS) {
        throw std::runtime_error("failed to read decoder frames");
    }
}

void decode(const void* encodedData, size_t encodedSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate) {
    decode(Encoding::UNKNOWN, encodedData, encodedSize, buffer, channelCount, frameRate);
}

void decode_wav(const void* wavData, size_t wavSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate) {
    decode(Encoding::WAV, wavData, wavSize, buffer, channelCount, frameRate);
}

void decode_mp3(const void* mp3Data, size_t mp3Size, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate) {
    decode(Encoding::MP3, mp3Data, mp3Size, buffer, channelCount, frameRate);
}

void decode_flac(const void* flacData, size_t flacSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate) {
    decode(Encoding::FLAC, flacData, flacSize, buffer, channelCount, frameRate);
}

void decode_ogg(const void* oggData, size_t oggSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate) {
    decode(Encoding::OGG, oggData, oggSize, buffer, channelCount, frameRate);
}

}// namespace duk::audio
