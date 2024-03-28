//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_CLIP_POOL_H
#define DUK_AUDIO_AUDIO_CLIP_POOL_H

#include <duk_audio/audio_device.h>
#include <duk_audio/clip/audio_clip.h>

#include <duk_resource/pool.h>

namespace duk::audio {

struct AudioClipPoolCreateInfo {
    AudioDevice* device;
};

class AudioClipPool : public duk::resource::PoolT<AudioClipResource> {
public:
    AudioClipPool(const AudioClipPoolCreateInfo& audioClipPoolCreateInfo);

    AudioClipResource create(duk::resource::Id id, Encoding encoding, const void* encodedData, size_t encodedSize);

    AudioClipResource create(duk::resource::Id id, const void* encodedData, size_t encodedSize);

    AudioClipResource create(duk::resource::Id id, Encoding encoding, const std::vector<uint8_t>& encodedData);

    AudioClipResource create(duk::resource::Id id, const std::vector<uint8_t>& encodedData);

private:
    AudioDevice* m_device;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_CLIP_POOL_H
