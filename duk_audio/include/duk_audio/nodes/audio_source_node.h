//
// Created by Ricardo on 24/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_SOURCE_NODE_H
#define DUK_AUDIO_AUDIO_SOURCE_NODE_H

#include <duk_audio/audio_graph.h>
#include <duk_audio/audio_source.h>

namespace duk::audio {

class AudioSourceNode : public AudioNode {
public:

    void process(void* output, uint32_t frameCount, uint32_t channelCount) override;

    void play(std::shared_ptr<AudioSource>& source);

private:
    std::vector<std::weak_ptr<AudioSource>> m_sources;
};


}

#endif //DUK_AUDIO_AUDIO_SOURCE_NODE_H
