//
// Created by Ricardo on 24/03/2024.
//

#include <duk_audio/audio_buffer.h>
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

static void process_buffer_float(const AudioBuffer* buffer, float* output, uint32_t frameCount, uint32_t channelCount, uint32_t& currentFrame, float frameRate, bool loop) {
    uint32_t framesRead = 0;
    float* dst = output;
    while (framesRead < frameCount && (currentFrame < buffer->frame_count() || loop)) {
        auto readAmount = buffer->read_float(dst, frameCount, currentFrame, channelCount, frameRate);
        dst += readAmount * channelCount;
        framesRead += readAmount;
        currentFrame += readAmount;

        if (loop && currentFrame >= buffer->frame_count()) {
            currentFrame = 0;
        }
    }
    if (framesRead < frameCount) {
        memset(output + framesRead * channelCount, 0, (frameCount - framesRead) * channelCount * sizeof(float));
    }
}

static void apply_volume(float* output, float volume, uint32_t frameCount, uint32_t channelCount) {
    for (uint32_t i = 0; i < frameCount; i++) {
        uint32_t frameIndex = i * channelCount;
        for (uint32_t c = 0; c < channelCount; c++) {
            output[frameIndex + c] *= volume;
        }
    }
}

static void process_slot(AudioSourceNode::Slot& slot, const std::shared_ptr<AudioBuffer>& buffer, void* output, uint32_t frameCount, uint32_t channelCount) {
    const uint32_t kBufferSampleCount = 1024;
    float outputBuffer[kBufferSampleCount];
    const uint32_t kBufferFrameCount = kBufferSampleCount / channelCount;

    float* dstFrames = (float*)output;
    uint32_t remainingFrames = frameCount;

    while (remainingFrames) {
        auto framesToRead = std::min(remainingFrames, kBufferFrameCount);

        process_buffer_float(buffer.get(), outputBuffer, framesToRead, channelCount, slot.deviceFrame, slot.deviceFrameRate, slot.deviceLoop);

        apply_volume(outputBuffer, slot.deviceVolume, framesToRead, channelCount);

        uint32_t dstOffset = (frameCount - remainingFrames) * channelCount;
        mix_float_samples(dstFrames + dstOffset, outputBuffer, framesToRead, channelCount);

        remainingFrames -= framesToRead;
    }

    slot.deviceVolume = slot.hostVolume;
    slot.deviceFrameRate = slot.hostFrameRate;
    slot.deviceLoop = slot.hostLoop;
    std::lock_guard lock(slot.syncMutex);
    if (slot.syncDeviceFrame) {
        slot.deviceFrame = slot.hostFrame;
        slot.syncDeviceFrame = false;
    } else {
        slot.hostFrame = slot.deviceFrame;
    }
}

static void update_slot(AudioSourceNode::Slot& slot, const std::shared_ptr<AudioBuffer>& buffer, float volume, float frameRate, bool loop, uint32_t priority) {
    slot.buffer = buffer;
    slot.priority = priority;
    slot.hostVolume = volume;
    slot.hostLoop = loop;
    slot.hostFrameRate = frameRate;
    slot.version++;
    std::lock_guard lock(slot.syncMutex);
    slot.hostFrame = 0;
    slot.syncDeviceFrame = true;
}

}// namespace detail

AudioSourceNode::AudioSourceNode(uint32_t slotCount)
    : m_slots(slotCount) {
}

void AudioSourceNode::process(void* output, uint32_t frameCount, uint32_t channelCount) {
    for (auto& slot: m_slots) {
        if (auto buffer = slot.buffer.lock()) {
            detail::process_slot(slot, buffer, output, frameCount, channelCount);
        }
    }
}

void AudioSourceNode::update() {
    for (auto& slot: m_slots) {
        if (auto buffer = slot.buffer.lock()) {
            if (slot.hostFrame == buffer->frame_count() && !slot.hostLoop) {
                // our slot is finished, free it
                slot.buffer.reset();
            }
        }
    }
}

AudioId AudioSourceNode::play(const std::shared_ptr<AudioBuffer>& buffer, float volume, float frameRate, bool loop, int32_t priority) {
    // try to find unused slot, if not found then replace the one with the lowest priority
    int32_t lowestPriority = priority;
    uint32_t lowestPriorityIndex = m_slots.size();
    for (uint32_t i = 0; i < m_slots.size(); i++) {
        auto& slot = m_slots[i];
        if (slot.buffer.expired()) {
            detail::update_slot(slot, buffer, volume, frameRate, loop, priority);
            return AudioId(slot.version, i);
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
        detail::update_slot(slot, buffer, volume, frameRate, loop, priority);
        return AudioId(slot.version, lowestPriorityIndex);
    }

    // slots.size() is an invalid id
    return AudioId(0, m_slots.size());
}

bool AudioSourceNode::is_playing(const AudioId& id) const {
    if (id.index >= m_slots.size()) {
        return false;
    }
    auto& slot = m_slots[id.index];
    if (slot.buffer.expired()) {
        return false;
    }
    return slot.version == id.version;
}

void AudioSourceNode::set_volume(const AudioId& id, float volume) {
    if (!is_playing(id)) {
        return;
    }
    auto& slot = m_slots[id.index];
    slot.hostVolume = volume;
}

float AudioSourceNode::volume(const AudioId& id) const {
    if (!is_playing(id)) {
        return 0.0f;
    }
    const auto& slot = m_slots[id.index];
    return slot.hostVolume;
}

void AudioSourceNode::set_frame_rate(const AudioId& id, float frameRate) {
    if (!is_playing(id)) {
        return;
    }
    auto& slot = m_slots[id.index];
    slot.hostFrameRate = frameRate;
}

float AudioSourceNode::frame_rate(const AudioId& id) const {
    if (!is_playing(id)) {
        return 0.0f;
    }
    const auto& slot = m_slots[id.index];
    return slot.hostFrameRate;
}

void AudioSourceNode::stop(const AudioId& id) {
    if (!is_playing(id)) {
        return;
    }
    auto& slot = m_slots[id.index];
    slot.buffer.reset();
}

}// namespace duk::audio
