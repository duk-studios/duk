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

void AudioPlayer::play(AudioDevice* device, const AudioClip* clip, float volume, float frameRate, bool loop, int32_t priority) {
    stop();
    m_id = device->play(clip->buffer(), volume, frameRate, loop, priority);
    m_device = device;
}

void AudioPlayer::stop() {
    if (!m_device) {
        return;
    }
    m_device->stop(m_id);
    m_id = kInvalidAudioId;
}

bool AudioPlayer::is_playing() const {
    if (!m_device) {
        return false;
    }
    return m_device->is_playing(m_id);
}

void AudioPlayer::set_volume(float volume) {
    if (!m_device) {
        return;
    }
    m_device->set_volume(m_id, volume);
}

float AudioPlayer::volume() const {
    if (!m_device) {
        return 0.0f;
    }
    return m_device->volume(m_id);
}

void AudioPlayer::set_frame_rate(float frameRate) {
    if (!m_device) {
        return;
    }
    m_device->set_frame_rate(m_id, frameRate);
}

float AudioPlayer::frame_rate() const {
    if (!m_device) {
        return 0.0f;
    }
    return m_device->frame_rate(m_id);
}

}// namespace duk::audio