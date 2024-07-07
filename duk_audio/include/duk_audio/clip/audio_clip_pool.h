//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_CLIP_POOL_H
#define DUK_AUDIO_AUDIO_CLIP_POOL_H

#include <duk_audio/audio_engine.h>
#include <duk_audio/clip/audio_clip.h>

#include <duk_resource/pool.h>

namespace duk::audio {

struct AudioClipPoolCreateInfo {
    Audio* engine;
};

class AudioClipPool : public duk::resource::PoolT<AudioClipResource> {
public:
    AudioClipPool(const AudioClipPoolCreateInfo& audioClipPoolCreateInfo);

    AudioClipResource create(duk::resource::Id id, Encoding encoding, const void* encodedData, size_t encodedSize);

    AudioClipResource create(duk::resource::Id id, const void* encodedData, size_t encodedSize);

    AudioClipResource create(duk::resource::Id id, Encoding encoding, const std::vector<uint8_t>& encodedData);

    AudioClipResource create(duk::resource::Id id, const std::vector<uint8_t>& encodedData);

private:
    Audio* m_engine;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_CLIP_POOL_H
