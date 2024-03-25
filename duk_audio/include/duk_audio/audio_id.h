//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_ID_H
#define DUK_AUDIO_ID_H

#include <cstdint>

namespace duk::audio {

struct AudioId {
    uint32_t version;
    uint32_t index;
};

}

#endif //DUK_AUDIO_ID_H
