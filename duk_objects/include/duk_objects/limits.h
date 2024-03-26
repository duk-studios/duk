/// 06/07/2023
/// limits.h

#ifndef DUK_OBJECTS_LIMITS_H
#define DUK_OBJECTS_LIMITS_H

#include <cstdint>

namespace duk::objects {

enum Limits : uint64_t {
    MAX_COMPONENTS = 128,
    MAX_OBJECTS = 65535,
};

}// namespace duk::objects

#endif// DUK_OBJECTS_LIMITS_H
