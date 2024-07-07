//
// Created by Ricardo on 04/07/2024.
//

#ifndef DUK_AUDIO_AUDIO_ENGINE_H
#define DUK_AUDIO_AUDIO_ENGINE_H

#include <duk_audio/audio_device.h>
#include <duk_audio/nodes/audio_source_node.h>
#include <duk_audio/clip/audio_clip.h>

namespace duk::audio {

struct AudioCreateInfo {
    uint32_t frameRate;
    uint32_t channelCount;
};

class Audio {
public:
    Audio(const AudioCreateInfo& audioCreateInfo);

    uint32_t frame_rate() const;

    uint32_t channel_count() const;

    void start();

    void stop();

    void update();

    AudioId play(const std::shared_ptr<AudioBuffer>& buffer, float volume = 1.0f, float frameRate = 1.0f, bool loop = false, int32_t priority = 0);

    AudioId play(const AudioClip* clip, float volume = 1.0f, float frameRate = 1.0f, bool loop = false, int32_t priority = 0);

    void stop(const AudioId& id);

    bool is_playing(const AudioId& id) const;

    void set_volume(const AudioId& id, float volume);

    float volume(const AudioId& id) const;

    void set_frame_rate(const AudioId& id, float frameRate);

    float frame_rate(const AudioId& id) const;

private:
    std::unique_ptr<AudioDevice> m_device;
    AudioGraph m_graph;
    AudioSourceNode* m_sourceNode;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_ENGINE_H
