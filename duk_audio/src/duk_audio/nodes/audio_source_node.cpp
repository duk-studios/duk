//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/nodes/audio_source_node.h>

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

}

void AudioSourceNode::process(void* output, uint32_t frameCount, uint32_t channelCount) {

    const uint32_t kBufferSampleCount = 1024;
    float buffer[kBufferSampleCount];
    const uint32_t kBufferFrameCount = kBufferSampleCount / channelCount;

    for (auto source : m_sources) {
        if (auto lockedSource = source.lock()) {
            float* dstFrames = (float*)output;
            uint32_t remainingFrames = frameCount;

            while (remainingFrames) {
                auto framesToRead = std::min(remainingFrames, kBufferFrameCount);
                auto framesRead = lockedSource->read_float(buffer, framesToRead, channelCount);
                if (framesRead == 0) {
                    break;
                }

                uint32_t dstOffset = (frameCount - remainingFrames) * channelCount;
                detail::mix_float_samples(dstFrames + dstOffset, buffer, framesRead, channelCount);

                remainingFrames -= framesRead;
            }
        }
    }
}

void AudioSourceNode::play(std::shared_ptr<AudioSource>& source) {
    m_sources.emplace_back(source);
}

}
