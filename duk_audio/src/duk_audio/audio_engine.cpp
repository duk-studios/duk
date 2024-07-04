//
// Created by Ricardo on 04/07/2024.
//

#include <duk_audio/audio_engine.h>

#include <duk_audio/clip/audio_clip.h>

namespace duk::audio {

AudioEngine::AudioEngine(const AudioEngineCreateInfo& audioEngineCreateInfo)
    : m_sourceNode(nullptr) {

    AudioDeviceCreateInfo audioDeviceCreateInfo = {};
    audioDeviceCreateInfo.backend = Backend::MINIAUDIO;
    audioDeviceCreateInfo.frameRate = audioEngineCreateInfo.frameRate;
    audioDeviceCreateInfo.channelCount = audioEngineCreateInfo.channelCount;
    audioDeviceCreateInfo.processor = &m_graph;
    m_device = AudioDevice::create(audioDeviceCreateInfo);

    m_sourceNode = m_graph.add<AudioSourceNode>(32);
}

uint32_t AudioEngine::frame_rate() const {
    return m_device->frame_rate();
}

uint32_t AudioEngine::channel_count() const {
    return m_device->channel_count();
}

void AudioEngine::start() {
    m_device->start();
}

void AudioEngine::stop() {
    m_device->stop();
}

void AudioEngine::update() {
    m_graph.update();
}

AudioId AudioEngine::play(const std::shared_ptr<AudioBuffer>& buffer, float volume, float frameRate, bool loop, int32_t priority) {
    return m_sourceNode->play(buffer, volume, frameRate, loop, priority);
}

AudioId AudioEngine::play(const AudioClip* clip, float volume, float frameRate, bool loop, int32_t priority) {
    return play(clip->buffer(), volume, frameRate, loop, priority);
}

void AudioEngine::stop(const AudioId& id) {
    m_sourceNode->stop(id);
}

bool AudioEngine::is_playing(const AudioId& id) const {
    return m_sourceNode->is_playing(id);
}

void AudioEngine::set_volume(const AudioId& id, float volume) {
    m_sourceNode->set_volume(id, volume);
}

float AudioEngine::volume(const AudioId& id) const {
    return m_sourceNode->volume(id);
}

void AudioEngine::set_frame_rate(const AudioId& id, float frameRate) {
    m_sourceNode->set_frame_rate(id, frameRate);
}

float AudioEngine::frame_rate(const AudioId& id) const {
    return m_sourceNode->frame_rate(id);
}

}
