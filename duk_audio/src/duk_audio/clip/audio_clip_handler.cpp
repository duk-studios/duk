//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/clip/audio_clip_handler.h>

namespace duk::audio {

AudioClipHandler::AudioClipHandler()
    : HandlerT("aud") {
}

bool AudioClipHandler::accepts(const std::string& extension) const {
    return extension == ".mp3" || extension == ".flac" || extension == ".ogg" || extension == ".wav";
}

duk::resource::Handle<AudioClip> AudioClipHandler::load_from_memory(AudioClipPool* pool, const resource::Id& id, const void* data, size_t size) {
    return pool->create(id, data, size);
}

}// namespace duk::audio
