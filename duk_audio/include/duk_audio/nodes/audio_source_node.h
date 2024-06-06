//
// Created by Ricardo on 24/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_SOURCE_NODE_H
#define DUK_AUDIO_AUDIO_SOURCE_NODE_H

#include <duk_audio/audio_graph.h>
#include <duk_audio/audio_id.h>

#include <atomic>
#include <mutex>

namespace duk::audio {

class AudioBuffer;

class AudioSourceNode : public AudioNode {
public:
    struct Slot {
        std::weak_ptr<AudioBuffer> buffer;
        uint32_t deviceFrame;
        float deviceFrameRate;
        float deviceVolume;
        bool deviceLoop;

        // flag to indicate the device that it should sync its frame with the host
        std::atomic<bool> syncDeviceFrame;
        std::atomic<uint32_t> hostFrame;
        std::atomic<bool> hostLoop;
        std::atomic<float> hostVolume;
        std::atomic<float> hostFrameRate;
        std::mutex syncMutex;

        int32_t priority;
        uint32_t version;
    };

public:
    AudioSourceNode(uint32_t slotCount);

    void process(void* output, uint32_t frameCount, uint32_t channelCount) override;

    void update() override;

    AudioId play(const std::shared_ptr<AudioBuffer>& buffer, float volume, float frameRate, bool loop, int32_t priority);

    void stop(const AudioId& id);

    bool is_playing(const AudioId& id) const;

    void set_volume(const AudioId& id, float volume);

    float volume(const AudioId& id) const;

    void set_frame_rate(const AudioId& id, float frameRate);

    float frame_rate(const AudioId& id) const;

private:
    std::vector<Slot> m_slots;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_SOURCE_NODE_H
