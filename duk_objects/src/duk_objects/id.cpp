//
// Created by Ricardo on 22/07/2024.
//

#include <duk_objects/id.h>

namespace duk::objects {

Id::Id()
    : Id(kInvalidObjectIndex, 0) {
}

Id::Id(uint32_t index, uint32_t version)
    : m_index(index)
    , m_version(version) {
}

uint32_t Id::index() const {
    return m_index;
}

uint32_t Id::version() const {
    return m_version;
}

}// namespace duk::objects