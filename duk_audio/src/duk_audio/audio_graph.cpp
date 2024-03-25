//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/audio_graph.h>

namespace duk::audio {

void AudioGraph::process(void* output, uint32_t frameCount, uint32_t channelCount) {
    if (!m_node) {
        return;
    }
    m_node->process(output, frameCount, channelCount);
}

void AudioGraph::update() {
    m_node->update();
}

}