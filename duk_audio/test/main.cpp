//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/audio_buffer.h>
#include <duk_audio/audio_device.h>
#include <duk_audio/decode.h>

#include <duk_tools/file.h>

#include <cmath>
#include <numbers>
#include <thread>

static std::shared_ptr<duk::audio::AudioBuffer> create_sin_buffer(float duration, float frequency, uint32_t frameRate, uint32_t channelCount) {
    auto buffer = std::make_shared<duk::audio::AudioBufferT<float>>((uint32_t)(duration * frameRate), channelCount);

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

    auto content = duk::tools::load_bytes(filepath);

    duk::audio::decode(content.data(), content.size(), buffer.get(), channelCount, frameRate);

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

    auto buffer = create_buffer("sample.flac", kChannelCount, kFrameRate);
    auto buffer2 = create_buffer("sample.wav", kChannelCount, kFrameRate);
    audioDevice->start();

    auto id1 = audioDevice->play(buffer, 1.0f, false, 100);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    auto id2 = audioDevice->play(buffer2, 0.5f, false);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    audioDevice->stop(id1);

    while (audioDevice->is_playing(id2)) {
        audioDevice->update();
    }

    audioDevice->stop();

    return 0;
}