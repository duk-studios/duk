//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/decode.h>

// use miniaudio decoding
#include <duk_audio/miniaudio/miniaudio_import.h>

#include <stdexcept>

namespace duk::audio {

void decode_wav(void* wavData, size_t wavSize, AudioBufferT<float>* buffer, uint32_t channelCount, uint32_t frameRate) {
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, channelCount, frameRate);
    decoderConfig.encodingFormat = ma_encoding_format_wav;

    ma_decoder decoder;
    ma_result result;
    result = ma_decoder_init_memory(wavData, wavSize, &decoderConfig, &decoder);
    if (result != MA_SUCCESS) {
        throw std::runtime_error("failed to initialize decoder for wav");
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

}