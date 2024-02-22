/// 06/07/2023
/// limits.h

#ifndef DUK_SCENE_LIMITS_H
#define DUK_SCENE_LIMITS_H

#include <cstdint>

namespace duk::scene {

enum Limits : uint64_t {
    MAX_COMPONENTS = 128,
    MAX_OBJECTS = 65535,
};

}

#endif // DUK_SCENE_LIMITS_H

