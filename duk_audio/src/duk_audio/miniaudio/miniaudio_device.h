//
// Created by Ricardo on 23/03/2024.
//

#ifndef DUK_AUDIO_MINIAUDIO_DEVICE_H
#define DUK_AUDIO_MINIAUDIO_DEVICE_H

#include <duk_audio/audio_device.h>
#include <duk_audio/miniaudio/miniaudio_import.h>

namespace duk {
namespace audio {

struct MiniaudioDeviceCreateInfo {
    AudioProcessor* processor;
    uint32_t frameRate;
    uint32_t channelCount;
};

class MiniaudioDevice : public AudioDevice {
public:
    explicit MiniaudioDevice(const MiniaudioDeviceCreateInfo& miniaudioEngineCreateInfo);

    ~MiniaudioDevice() override;

    uint32_t frame_rate() const override;

    uint32_t channel_count() const override;

    void start() override;

    void stop() override;

    void data_callback(void* output, const void* input, uint32_t frameCount) const;

private:
    AudioProcessor* m_processor;
    uint32_t m_frameRate;
    uint32_t m_channelCount;
    ma_device m_device;
};

}// namespace audio
}// namespace duk

#endif//DUK_AUDIO_MINIAUDIO_DEVICE_H
