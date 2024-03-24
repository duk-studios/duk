//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/nodes/audio_source_node.h>

#include <algorithm>

namespace duk::audio {

namespace detail {

static void mix_float_samples(float* dst, const float* src, uint32_t frameCount, uint32_t channelCount) {
    for (uint32_t i = 0; i < frameCount; i++) {
        uint32_t frameIndex = i * channelCount;
        for (uint32_t c = 0; c < channelCount; c++) {
            dst[frameIndex + c] += src[frameIndex + c];
        }
    }
}

static void process_source(const std::shared_ptr<AudioSource>& source, void* output, uint32_t frameCount, uint32_t channelCount) {

    const uint32_t kBufferSampleCount = 1024;
    float buffer[kBufferSampleCount];
    const uint32_t kBufferFrameCount = kBufferSampleCount / channelCount;

    float* dstFrames = (float*)output;
    uint32_t remainingFrames = frameCount;

    while (remainingFrames) {
        auto framesToRead = std::min(remainingFrames, kBufferFrameCount);
        auto framesRead = source->read_float(buffer, framesToRead, channelCount);
        if (framesRead == 0) {
            break;
        }

        uint32_t dstOffset = (frameCount - remainingFrames) * channelCount;
        mix_float_samples(dstFrames + dstOffset, buffer, framesRead, channelCount);

        remainingFrames -= framesRead;
    }
}

}

AudioSourceNode::AudioSourceNode(uint32_t slotCount) :
    m_slots(slotCount) {

}

void AudioSourceNode::process(void* output, uint32_t frameCount, uint32_t channelCount) {
    for (auto slot : m_slots) {
        if (auto lockedSource = slot.source.lock()) {
            detail::process_source(lockedSource, output, frameCount, channelCount);
        }
    }
}

void AudioSourceNode::play(std::shared_ptr<AudioSource>& source, int32_t priority) {

    // try to find unused slot, if not found then replace the one with the lowest priority
    int32_t lowestPriority = priority;
    uint32_t lowestPriorityIndex = m_slots.size();
    for (uint32_t i = 0; i < m_slots.size(); i++) {
        auto& slot = m_slots[i];
        if (slot.source.expired()) {
            slot.source = source;
            slot.priority = priority;
            // this source will start playing on the next process call
            return;
        }
        // we may replace this source
        if (slot.priority < lowestPriority) {
            lowestPriority = slot.priority;
            lowestPriorityIndex = i;
        }
    }

    // if lowestPriorityIndex is valid, it will be the index to the lowest priority slot
    if (lowestPriorityIndex < m_slots.size()) {
        auto& slot = m_slots[lowestPriorityIndex];
        slot.source = source;
        slot.priority = lowestPriorityIndex;
    }
}

}
