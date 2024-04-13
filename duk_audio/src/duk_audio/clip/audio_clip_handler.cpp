//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/clip/audio_clip_handler.h>

#include <duk_tools/file.h>

namespace duk::audio {

AudioClipHandler::AudioClipHandler(const AudioClipHandlerCreateInfo& audioClipHandlerCreateInfo)
    : m_audioClipPool(audioClipHandlerCreateInfo.audioClipPool) {
}

const std::string& AudioClipHandler::tag() const {
    static const std::string tag = "aud";
    return tag;
}

void AudioClipHandler::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument(fmt::format("failed to load AudioClip, path ({}) does not exist", path.string()));
    }
    m_audioClipPool->create(id, duk::tools::File::load_bytes(path.string().c_str()));
}

}// namespace duk::audio
