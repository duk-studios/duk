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
    miniaudioDeviceCreateInfo.processor = audioDeviceCreateInfo.processor;
    miniaudioDeviceCreateInfo.frameRate = audioDeviceCreateInfo.frameRate;
    miniaudioDeviceCreateInfo.channelCount = audioDeviceCreateInfo.channelCount;
    return std::make_unique<MiniaudioDevice>(miniaudioDeviceCreateInfo);
}

}// namespace detail

std::unique_ptr<AudioDevice> AudioDevice::create(const AudioDeviceCreateInfo& audioDeviceCreateInfo) {
    switch (audioDeviceCreateInfo.backend) {
        case Backend::MINIAUDIO:
            return detail::create_miniaudio_device(audioDeviceCreateInfo);
        default:
            throw std::invalid_argument("unsupported audio backend");
    }
}

AudioDevice::~AudioDevice() = default;

}// namespace duk::audio