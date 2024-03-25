//
// Created by Ricardo on 24/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_GRAPH_H
#define DUK_AUDIO_AUDIO_GRAPH_H

#include <duk_macros/assert.h>

#include <cstdint>
#include <vector>
#include <memory>

namespace duk::audio {

class AudioNode {
public:
    virtual ~AudioNode() = default;

    virtual void process(void* output, uint32_t frameCount, uint32_t channelCount) = 0;

    virtual void update() = 0;
};

class AudioGraph {
public:

    void process(void* output, uint32_t frameCount, uint32_t channelCount);

    void update();

    template<typename TNode, typename ...Args>
    TNode* add(Args&&... args);

private:
    std::unique_ptr<AudioNode> m_node;
};

template<typename TNode, typename... Args>
TNode* AudioGraph::add(Args&& ... args) {
    DUK_ASSERT(!m_node);
    m_node = std::make_unique<TNode>(std::forward<Args>(args)...);
    return (TNode*)m_node.get();
}

}

#endif //DUK_AUDIO_AUDIO_GRAPH_H
