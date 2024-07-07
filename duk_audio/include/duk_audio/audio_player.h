//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_PLAYER_H
#define DUK_AUDIO_AUDIO_PLAYER_H

#include <duk_audio/audio_id.h>

#include <cstdint>

namespace duk::audio {

class Audio;
class AudioClip;

class AudioPlayer {
public:
    AudioPlayer();

    ~AudioPlayer();

    void play(Audio* engine, const AudioClip* clip, float volume = 1.0f, float frameRate = 1.0f, bool loop = false, int32_t priority = 0);

    void stop();

    bool is_playing() const;

    void set_volume(float volume);

    float volume() const;

    void set_frame_rate(float frameRate);

    float frame_rate() const;

private:
    Audio* m_engine;
    AudioId m_id;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_PLAYER_H
