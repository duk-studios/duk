//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/audio_engine.h>
#include <duk_audio/audio_player.h>
#include <duk_audio/clip/audio_clip.h>

namespace duk::audio {

AudioPlayer::AudioPlayer()
    : m_engine(nullptr)
    , m_id(kInvalidAudioId) {
}

AudioPlayer::~AudioPlayer() {
    stop();
}

void AudioPlayer::play(AudioEngine* engine, const AudioClip* clip, float volume, float frameRate, bool loop, int32_t priority) {
    stop();
    m_id = engine->play(clip->buffer(), volume, frameRate, loop, priority);
    m_engine = engine;
}

void AudioPlayer::stop() {
    if (!m_engine) {
        return;
    }
    m_engine->stop(m_id);
    m_id = kInvalidAudioId;
}

bool AudioPlayer::is_playing() const {
    if (!m_engine) {
        return false;
    }
    return m_engine->is_playing(m_id);
}

void AudioPlayer::set_volume(float volume) {
    if (!m_engine) {
        return;
    }
    m_engine->set_volume(m_id, volume);
}

float AudioPlayer::volume() const {
    if (!m_engine) {
        return 0.0f;
    }
    return m_engine->volume(m_id);
}

void AudioPlayer::set_frame_rate(float frameRate) {
    if (!m_engine) {
        return;
    }
    m_engine->set_frame_rate(m_id, frameRate);
}

float AudioPlayer::frame_rate() const {
    if (!m_engine) {
        return 0.0f;
    }
    return m_engine->frame_rate(m_id);
}

}// namespace duk::audio