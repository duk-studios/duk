//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/clip/audio_clip_pool.h>

namespace duk::audio {

AudioClipPool::AudioClipPool(const AudioClipPoolCreateInfo& audioClipPoolCreateInfo)
    : m_device(audioClipPoolCreateInfo.device) {
}

AudioClipResource AudioClipPool::create(duk::resource::Id id, Encoding encoding, const void* encodedData, size_t encodedSize) {
    AudioClipCreateInfo audioClipCreateInfo = {};
    audioClipCreateInfo.encodedData = encodedData;
    audioClipCreateInfo.encodedSize = encodedSize;
    audioClipCreateInfo.frameRate = m_device->frame_rate();
    audioClipCreateInfo.channelCount = m_device->channel_count();
    audioClipCreateInfo.encoding = encoding;

    return insert(id, std::make_shared<AudioClip>(audioClipCreateInfo));
}

AudioClipResource AudioClipPool::create(duk::resource::Id id, const void* encodedData, size_t encodedSize) {
    return create(id, Encoding::UNKNOWN, encodedData, encodedSize);
}

AudioClipResource AudioClipPool::create(duk::resource::Id id, Encoding encoding, const std::vector<uint8_t>& encodedData) {
    return create(id, encoding, encodedData.data(), encodedData.size());
}

AudioClipResource AudioClipPool::create(duk::resource::Id id, const std::vector<uint8_t>& encodedData) {
    return create(id, Encoding::UNKNOWN, encodedData.data(), encodedData.size());
}

}// namespace duk::audio