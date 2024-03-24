//
// Created by Ricardo on 23/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_DEVICE
#define DUK_AUDIO_AUDIO_DEVICE

#include <memory>

namespace duk::audio {

class AudioSource;

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

    static std::shared_ptr<AudioDevice> create(const AudioDeviceCreateInfo& audioEngineCreateInfo);

    virtual ~AudioDevice();

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void play(std::shared_ptr<AudioSource>& source, int32_t priority = 0) = 0;

};

}

#endif //DUK_AUDIO_AUDIO_DEVICE
