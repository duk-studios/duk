//
// Created by Ricardo on 04/07/2024.
//

#include <duk_audio/audio_engine.h>

#include <duk_audio/clip/audio_clip.h>

namespace duk::audio {

Audio::Audio(const AudioCreateInfo& audioCreateInfo)
    : m_sourceNode(nullptr) {
    AudioDeviceCreateInfo audioDeviceCreateInfo = {};
    audioDeviceCreateInfo.backend = Backend::MINIAUDIO;
    audioDeviceCreateInfo.frameRate = audioCreateInfo.frameRate;
    audioDeviceCreateInfo.channelCount = audioCreateInfo.channelCount;
    audioDeviceCreateInfo.processor = &m_graph;
    m_device = AudioDevice::create(audioDeviceCreateInfo);

    m_sourceNode = m_graph.add<AudioSourceNode>(32);
}

uint32_t Audio::frame_rate() const {
    return m_device->frame_rate();
}

uint32_t Audio::channel_count() const {
    return m_device->channel_count();
}

void Audio::start() {
    m_device->start();
}

void Audio::stop() {
    m_device->stop();
}

void Audio::update() {
    m_graph.update();
}

AudioId Audio::play(const std::shared_ptr<AudioBuffer>& buffer, float volume, float frameRate, bool loop, int32_t priority) {
    return m_sourceNode->play(buffer, volume, frameRate, loop, priority);
}

AudioId Audio::play(const AudioClip* clip, float volume, float frameRate, bool loop, int32_t priority) {
    return play(clip->buffer(), volume, frameRate, loop, priority);
}

void Audio::stop(const AudioId& id) {
    m_sourceNode->stop(id);
}

bool Audio::is_playing(const AudioId& id) const {
    return m_sourceNode->is_playing(id);
}

void Audio::set_volume(const AudioId& id, float volume) {
    m_sourceNode->set_volume(id, volume);
}

float Audio::volume(const AudioId& id) const {
    return m_sourceNode->volume(id);
}

void Audio::set_frame_rate(const AudioId& id, float frameRate) {
    m_sourceNode->set_frame_rate(id, frameRate);
}

float Audio::frame_rate(const AudioId& id) const {
    return m_sourceNode->frame_rate(id);
}

}// namespace duk::audio
