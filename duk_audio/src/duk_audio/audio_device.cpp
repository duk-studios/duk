//
// Created by Ricardo on 23/03/2024.
//
#include <duk_audio/audio_device.h>
#include <duk_audio/miniaudio/miniaudio_device.h>

namespace duk::audio {

namespace detail {

std::shared_ptr<AudioDevice> create_miniaudio_device(const AudioDeviceCreateInfo& audioDeviceCreateInfo) {
    MiniaudioDeviceCreateInfo miniaudioDeviceCreateInfo = {};
    miniaudioDeviceCreateInfo.frameRate = audioDeviceCreateInfo.frameRate;
    miniaudioDeviceCreateInfo.channelCount = audioDeviceCreateInfo.channelCount;
    return std::make_shared<MiniaudioDevice>(miniaudioDeviceCreateInfo);
}

}

std::shared_ptr<AudioDevice> AudioDevice::create(const AudioDeviceCreateInfo& audioEngineCreateInfo) {
    switch (audioEngineCreateInfo.backend) {
        case Backend::MINIAUDIO:
            return detail::create_miniaudio_device(audioEngineCreateInfo);
        default:
            throw std::invalid_argument("unsupported audio backend");
    }
}

AudioDevice::~AudioDevice() = default;

}