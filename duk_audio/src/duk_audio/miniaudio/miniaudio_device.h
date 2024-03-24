//
// Created by Ricardo on 23/03/2024.
//

#ifndef DUK_AUDIO_MINIAUDIO_DEVICE_H
#define DUK_AUDIO_MINIAUDIO_DEVICE_H

#include <duk_audio/audio_device.h>
#include <duk_audio/audio_graph.h>
#include <duk_audio/audio_source.h>
#include <duk_audio/nodes/audio_source_node.h>
#include <duk_audio/miniaudio/miniaudio_import.h>
#include <duk_audio/miniaudio/miniaudio_import.h>

namespace duk {
namespace audio {

struct MiniaudioDeviceCreateInfo {
    uint32_t frameRate;
    uint32_t channelCount;
};

class MiniaudioDevice : public AudioDevice {
public:

    explicit MiniaudioDevice(const MiniaudioDeviceCreateInfo& miniaudioEngineCreateInfo);

    ~MiniaudioDevice() override;

    void start() override;

    void stop() override;

    void play(std::shared_ptr<AudioSource>& source, int32_t priority) override;

    void data_callback(void* output, const void* input, uint32_t frameCount);

private:
    uint32_t m_frameRate;
    uint32_t m_channelCount;
    ma_device m_device;
    AudioGraph m_graph;
    AudioSourceNode* m_sourceNode;

};

} // audio
} // duk

#endif //DUK_AUDIO_MINIAUDIO_DEVICE_H
