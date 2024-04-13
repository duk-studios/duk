//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_CLIP_HANDLER_H
#define DUK_AUDIO_AUDIO_CLIP_HANDLER_H

#include <duk_resource/handler.h>

#include <duk_audio/clip/audio_clip_pool.h>

namespace duk::audio {

struct AudioClipHandlerCreateInfo {
    AudioClipPool* audioClipPool;
};

class AudioClipHandler : public duk::resource::ResourceHandler {
public:
    explicit AudioClipHandler(const AudioClipHandlerCreateInfo& audioClipHandlerCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

private:
    AudioClipPool* m_audioClipPool;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_CLIP_HANDLER_H
