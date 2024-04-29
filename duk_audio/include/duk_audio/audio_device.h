//
// Created by Ricardo on 23/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_DEVICE
#define DUK_AUDIO_AUDIO_DEVICE

#include <duk_audio/audio_id.h>

#include <memory>

namespace duk::audio {

class AudioBuffer;
class AudioClip;

enum class Backend {
    MINIAUDIO
};

struct AudioDeviceCreateInfo {
    Backend backend;
    size_t frameRate;
    size_t channelCount;
};

class AudioDevice {
public:
    static std::unique_ptr<AudioDevice> create(const AudioDeviceCreateInfo& audioEngineCreateInfo);

    virtual ~AudioDevice();

    virtual uint32_t frame_rate() const = 0;

    virtual uint32_t channel_count() const = 0;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void update() = 0;

    virtual AudioId play(const std::shared_ptr<AudioBuffer>& buffer, float volume = 1.0f, bool loop = false, int32_t priority = 0) = 0;

    AudioId play(const AudioClip* clip, float volume = 1.0f, bool loop = false, int32_t priority = 0);

    virtual void stop(const AudioId& id) = 0;

    virtual bool is_playing(const AudioId& id) const = 0;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_DEVICE
