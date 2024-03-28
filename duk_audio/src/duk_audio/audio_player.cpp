//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/audio_device.h>
#include <duk_audio/audio_player.h>
#include <duk_audio/clip/audio_clip.h>

namespace duk::audio {

AudioPlayer::AudioPlayer()
    : m_device(nullptr)
    , m_id(kInvalidAudioId) {
}

AudioPlayer::~AudioPlayer() {
    stop();
}

void AudioPlayer::play(AudioDevice* device, const AudioClip* clip, bool loop, int32_t priority) {
    stop();
    m_id = device->play(clip->buffer(), loop, priority);
    m_device = device;
}

void AudioPlayer::stop() {
    if (!m_device) {
        return;
    }
    m_device->stop(m_id);
    m_id = kInvalidAudioId;
}

}// namespace duk::audio