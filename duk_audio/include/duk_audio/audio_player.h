//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_PLAYER_H
#define DUK_AUDIO_AUDIO_PLAYER_H

#include <duk_audio/audio_id.h>

#include <cstdint>

namespace duk::audio {

class AudioDevice;
class AudioClip;

class AudioPlayer {
public:
    AudioPlayer();

    ~AudioPlayer();

    void play(AudioDevice* device, const AudioClip* clip, float volume = 0, bool loop = false, int32_t priority = 0);

    void stop();

private:
    AudioDevice* m_device;
    AudioId m_id;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_PLAYER_H
