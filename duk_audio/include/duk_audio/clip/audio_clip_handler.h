//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_CLIP_HANDLER_H
#define DUK_AUDIO_AUDIO_CLIP_HANDLER_H

#include <duk_resource/handler.h>

#include <duk_audio/clip/audio_clip_pool.h>

namespace duk::audio {

class AudioClipHandler : public duk::resource::ResourceHandlerT<AudioClipPool> {
public:
    AudioClipHandler();

protected:
    void load(AudioClipPool* pool, const resource::Id& id, const std::filesystem::path& path) override;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_CLIP_HANDLER_H
