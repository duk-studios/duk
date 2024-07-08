//
// Created by Ricardo on 23/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_DEVICE
#define DUK_AUDIO_AUDIO_DEVICE

#include <duk_audio/audio_processor.h>

#include <memory>

namespace duk::audio {

enum class Backend {
    MINIAUDIO
};

struct AudioDeviceCreateInfo {
    AudioProcessor* processor;
    Backend backend;
    size_t frameRate;
    size_t channelCount;
};

class AudioDevice {
public:
    static std::unique_ptr<AudioDevice> create(const AudioDeviceCreateInfo& audioDeviceCreateInfo);

    virtual ~AudioDevice();

    virtual uint32_t frame_rate() const = 0;

    virtual uint32_t channel_count() const = 0;

    virtual void start() = 0;

    virtual void stop() = 0;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_DEVICE
