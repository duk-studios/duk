//
// Created by Ricardo on 25/03/2024.
//

#include <duk_audio/clip/audio_clip_handler.h>

#include "duk_audio/audio_engine.h"

namespace duk::audio {

AudioClipHandler::AudioClipHandler()
    : HandlerT("aud") {
}

bool AudioClipHandler::accepts(const std::string& extension) const {
    return extension == ".mp3" || extension == ".flac" || extension == ".ogg" || extension == ".wav";
}

std::shared_ptr<AudioClip> AudioClipHandler::load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) {
    const auto audio = globals->get<Audio>();

    AudioClipCreateInfo audioClipCreateInfo = {};
    audioClipCreateInfo.encodedData = data;
    audioClipCreateInfo.encodedSize = size;
    audioClipCreateInfo.frameRate = audio->frame_rate();
    audioClipCreateInfo.channelCount = audio->channel_count();
    audioClipCreateInfo.encoding = Encoding::UNKNOWN;

    return std::make_shared<AudioClip>(audioClipCreateInfo);
}

}// namespace duk::audio
