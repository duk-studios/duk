//
// Created by Ricardo on 23/03/2024.
//
#include <duk_audio/audio_device.h>
#include <duk_audio/clip/audio_clip.h>
#include <duk_audio/miniaudio/miniaudio_device.h>

namespace duk::audio {

namespace detail {

std::unique_ptr<AudioDevice> create_miniaudio_device(const AudioDeviceCreateInfo& audioDeviceCreateInfo) {
    MiniaudioDeviceCreateInfo miniaudioDeviceCreateInfo = {};
    miniaudioDeviceCreateInfo.frameRate = audioDeviceCreateInfo.frameRate;
    miniaudioDeviceCreateInfo.channelCount = audioDeviceCreateInfo.channelCount;
    return std::make_unique<MiniaudioDevice>(miniaudioDeviceCreateInfo);
}

}// namespace detail

std::unique_ptr<AudioDevice> AudioDevice::create(const AudioDeviceCreateInfo& audioEngineCreateInfo) {
    switch (audioEngineCreateInfo.backend) {
        case Backend::MINIAUDIO:
            return detail::create_miniaudio_device(audioEngineCreateInfo);
        default:
            throw std::invalid_argument("unsupported audio backend");
    }
}

AudioDevice::~AudioDevice() = default;

AudioId AudioDevice::play(const AudioClip* clip, float volume, float frameRate, bool loop, int32_t priority) {
    return play(clip->buffer(), volume, frameRate, loop, priority);
}

}// namespace duk::audio