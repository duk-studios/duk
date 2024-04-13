//
// Created by Ricardo on 13/04/2024.
//

#include <duk_audio/register_types.h>

#include <duk_audio/clip/audio_clip_handler.h>

namespace duk::audio {

void register_types() {
    duk::resource::register_handler<AudioClipHandler>();
}

}