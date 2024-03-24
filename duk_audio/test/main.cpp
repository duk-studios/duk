//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/audio_device.h>
#include <duk_audio/audio_buffer.h>
#include <duk_audio/audio_source.h>
#include <duk_audio/decode.h>

#include <duk_tools/file.h>

#include <iostream>
#include <cmath>

static std::shared_ptr<duk::audio::AudioBuffer> create_sin_buffer(float duration, float frequency, uint32_t frameRate, uint32_t channelCount) {
    auto buffer = std::make_shared<duk::audio::AudioBufferT<float>>(duration * frameRate, channelCount);

    float* frame = buffer->data();
    for (uint32_t i = 0; i < buffer->frame_count(); i++) {
        float percent = (float)i / (float)(frameRate - 1);
        float angle = percent * std::numbers::pi_v<float> * 2;
        float sample = sinf(angle * frequency);

        for (uint32_t channel = 0; channel < channelCount; channel++) {
            frame[channel] = sample;
        }
        frame += channelCount;
    }
    return buffer;
}

static std::shared_ptr<duk::audio::AudioBuffer> create_buffer(const char* filepath, uint32_t channelCount, uint32_t frameRate) {
    auto buffer = std::make_shared<duk::audio::AudioBufferT<float>>();

    auto content = duk::tools::File::load_bytes(filepath);

    duk::audio::decode_wav(content.data(), content.size(), buffer.get(), channelCount, frameRate);

    return buffer;
}

int main() {

    const float kFrameRate = 48000;
    const uint32_t kChannelCount = 2;

    duk::audio::AudioDeviceCreateInfo audioDeviceCreateInfo = {};
    audioDeviceCreateInfo.backend = duk::audio::Backend::MINIAUDIO;
    audioDeviceCreateInfo.channelCount = kChannelCount;
    audioDeviceCreateInfo.frameRate = kFrameRate;

    auto audioDevice = duk::audio::AudioDevice::create(audioDeviceCreateInfo);

    auto buffer = create_buffer("file_example_WAV_1MG.wav", kChannelCount, kFrameRate);
    auto audioSource = std::make_shared<duk::audio::AudioSource>(buffer);

    audioDevice->start();

    audioDevice->play(audioSource);

    while (audioSource->is_playing()) {}

    audioDevice->stop();

    return 0;
}