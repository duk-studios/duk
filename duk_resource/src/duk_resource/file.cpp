//
// Created by Ricardo on 13/04/2024.
//

#include <duk_resource/file.h>

namespace duk::resource {

bool operator==(const ResourceFile& lhs, const ResourceFile& rhs) {
    return lhs.id == rhs.id;
}

bool operator<(const ResourceFile& lhs, const ResourceFile& rhs) {
    return lhs.id < rhs.id;
}

}// namespace duk::resource