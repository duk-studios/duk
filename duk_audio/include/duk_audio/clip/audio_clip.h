//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_CLIP_H
#define DUK_AUDIO_AUDIO_CLIP_H

#include <duk_audio/audio_buffer.h>
#include <duk_audio/decode.h>

#include <duk_resource/resource.h>

#include <memory>

namespace duk::audio {

struct AudioClipCreateInfo {
    Encoding encoding;
    const void* encodedData;
    size_t encodedSize;
    uint32_t channelCount;
    uint32_t frameRate;
};

class AudioClip {
public:
    AudioClip(const AudioClipCreateInfo& audioClipCreateInfo);

    const std::shared_ptr<AudioBuffer>& buffer() const;

private:
    std::shared_ptr<AudioBuffer> m_buffer;
};

using AudioClipResource = duk::resource::Handle<AudioClip>;

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_CLIP_H
