//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/audio_device.h>
#include <duk_audio/audio_buffer.h>
#include <duk_audio/audio_source.h>

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

int main() {

    const float kFrameRate = 48000;
    const uint32_t kChannelCount = 2;

    duk::audio::AudioDeviceCreateInfo audioDeviceCreateInfo = {};
    audioDeviceCreateInfo.backend = duk::audio::Backend::MINIAUDIO;
    audioDeviceCreateInfo.channelCount = kChannelCount;
    audioDeviceCreateInfo.frameRate = kFrameRate;

    auto audioDevice = duk::audio::AudioDevice::create(audioDeviceCreateInfo);

    auto buffer1 = create_sin_buffer(10.0, 2000, kFrameRate, kChannelCount);
    auto buffer2 = create_sin_buffer(5.0, 450, kFrameRate, kChannelCount);
    auto audioSource1 = std::make_shared<duk::audio::AudioSource>(buffer1);
    auto audioSource2 = std::make_shared<duk::audio::AudioSource>(buffer2);

    audioDevice->start();

    while (true) {
        audioDevice->play(audioSource1);
        audioDevice->play(audioSource2);

        char c = std::cin.get();
        if (c == 'q' || c == 'Q')
            break;
    }


    audioDevice->stop();

    return 0;
}