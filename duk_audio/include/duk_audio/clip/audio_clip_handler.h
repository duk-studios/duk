//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_CLIP_HANDLER_H
#define DUK_AUDIO_AUDIO_CLIP_HANDLER_H

#include <duk_resource/handler.h>

#include <duk_audio/clip/audio_clip.h>

namespace duk::audio {

class AudioClipHandler : public duk::resource::HandlerT<AudioClip> {
public:
    AudioClipHandler();

    bool accepts(const std::string& extension) const override;

protected:
    std::shared_ptr<AudioClip> load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) override;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_CLIP_HANDLER_H
