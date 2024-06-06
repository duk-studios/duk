//
// Created by Ricardo on 23/03/2024.
//

#include <duk_audio/miniaudio/miniaudio_device.h>

#include <fmt/format.h>

namespace duk {
namespace audio {

namespace detail {

static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    auto miniaudioDevice = reinterpret_cast<MiniaudioDevice*>(device->pUserData);
    if (miniaudioDevice) {
        miniaudioDevice->data_callback(output, input, frameCount);
    }
}

}// namespace detail

MiniaudioDevice::MiniaudioDevice(const MiniaudioDeviceCreateInfo& miniaudioEngineCreateInfo)
    : m_frameRate(miniaudioEngineCreateInfo.frameRate)
    , m_channelCount(miniaudioEngineCreateInfo.channelCount) {
    ma_device_config config;
    config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = m_channelCount;
    config.sampleRate = m_frameRate;
    config.dataCallback = detail::data_callback;
    config.pUserData = this;

    auto result = ma_device_init(NULL, &config, &m_device);
    if (result != MA_SUCCESS) {
        throw std::runtime_error(fmt::format("failed to initialize miniaudio device, error code: {}", (int)result));
    }

    m_sourceNode = m_graph.add<AudioSourceNode>(32);
}

MiniaudioDevice::~MiniaudioDevice() {
    ma_device_uninit(&m_device);
}

uint32_t MiniaudioDevice::frame_rate() const {
    return m_frameRate;
}

uint32_t MiniaudioDevice::channel_count() const {
    return m_channelCount;
}

void MiniaudioDevice::start() {
    ma_device_start(&m_device);
}

void MiniaudioDevice::stop() {
    ma_device_stop(&m_device);
}

void MiniaudioDevice::update() {
    m_graph.update();
}

AudioId MiniaudioDevice::play(const std::shared_ptr<AudioBuffer>& buffer, float volume, float frameRate, bool loop, int32_t priority) {
    return m_sourceNode->play(buffer, volume, frameRate, loop, priority);
}

void MiniaudioDevice::stop(const AudioId& id) {
    m_sourceNode->stop(id);
}

bool MiniaudioDevice::is_playing(const AudioId& id) const {
    return m_sourceNode->is_playing(id);
}

void MiniaudioDevice::set_volume(const AudioId& id, float volume) {
    m_sourceNode->set_volume(id, volume);
}

float MiniaudioDevice::volume(const AudioId& id) const {
    return m_sourceNode->volume(id);
}

void MiniaudioDevice::set_frame_rate(const AudioId& id, float frameRate) {
    m_sourceNode->set_frame_rate(id, frameRate);
}

float MiniaudioDevice::frame_rate(const AudioId& id) const {
    return m_sourceNode->frame_rate(id);
}

void MiniaudioDevice::data_callback(void* output, const void* input, ma_uint32 frameCount) {
    m_graph.process(output, frameCount, m_channelCount);
}

}// namespace audio
}// namespace duk