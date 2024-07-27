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

bool Id::operator==(const Id& rhs) const {
    return m_index == rhs.m_index && m_version == rhs.m_version;
}

bool Id::operator!=(const Id& rhs) const {
    return !(*this == rhs);
}

}// namespace duk::objects