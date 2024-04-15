//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/clip/audio_clip_handler.h>

#include <duk_tools/file.h>

namespace duk::audio {

AudioClipHandler::AudioClipHandler()
    : ResourceHandlerT("aud") {
}

bool AudioClipHandler::accepts(const std::string& extension) const {
    return extension == ".mp3" || extension == ".flac" || extension == ".ogg" || extension == ".wav";
}

void AudioClipHandler::load(AudioClipPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument(fmt::format("failed to load AudioClip, path ({}) does not exist", path.string()));
    }
    pool->create(id, duk::tools::load_bytes(path.string().c_str()));
}
}// namespace duk::audio
