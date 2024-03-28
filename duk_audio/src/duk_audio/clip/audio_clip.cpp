//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/clip/audio_clip.h>

namespace duk::audio {

AudioClip::AudioClip(const AudioClipCreateInfo& audioClipCreateInfo) {
    auto buffer = std::make_shared<AudioBufferT<float>>();
    duk::audio::decode(audioClipCreateInfo.encoding, audioClipCreateInfo.encodedData, audioClipCreateInfo.encodedSize, buffer.get(), audioClipCreateInfo.channelCount, audioClipCreateInfo.frameRate);
    m_buffer = buffer;
}

const std::shared_ptr<AudioBuffer>& AudioClip::buffer() const {
    return m_buffer;
}

}// namespace duk::audio
