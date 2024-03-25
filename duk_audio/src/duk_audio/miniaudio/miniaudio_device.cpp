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

}


MiniaudioDevice::MiniaudioDevice(const MiniaudioDeviceCreateInfo& miniaudioEngineCreateInfo)
    : m_frameRate(miniaudioEngineCreateInfo.frameRate)
    , m_channelCount(miniaudioEngineCreateInfo.channelCount) {

    ma_device_config config;
    config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = miniaudioEngineCreateInfo.channelCount;
    config.sampleRate = miniaudioEngineCreateInfo.frameRate;
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

void MiniaudioDevice::start() {
    ma_device_start(&m_device);
}

void MiniaudioDevice::stop() {
    ma_device_stop(&m_device);
}

void MiniaudioDevice::update() {
    m_graph.update();
}

AudioId MiniaudioDevice::play(std::shared_ptr<AudioBuffer>& buffer, bool loop, int32_t priority) {
    return m_sourceNode->play(buffer, loop, priority);
}

void MiniaudioDevice::stop(const AudioId& id) {
    m_sourceNode->stop(id);
}

bool MiniaudioDevice::is_playing(const AudioId& id) const {
    return m_sourceNode->is_playing(id);
}

void MiniaudioDevice::data_callback(void* output, const void* input, ma_uint32 frameCount) {
    m_graph.process(output, frameCount, m_channelCount);
}


} // audio
} // duk