//
// Created by rov on 11/20/2023.
//

#include <duk_resource/resource.h>

namespace duk::resource {

bool Id::operator==(const Id& rhs) const {
    return m_id == rhs.m_id;
}

bool Id::operator!=(const Id& rhs) const {
    return m_id != rhs.m_id;
}

bool Id::operator<(const Id& rhs) const {
    return m_id < rhs.m_id;
}

uint64_t Id::value() const {
    return m_id;
}

bool Id::valid() const {
    return m_id != 0;
}

Resource::~Resource() = default;

Id Resource::id() const {
    return m_id;
}

void Resource::reset(Id id) {
    m_id = id;
}

Resource::Resource(Id id)
    : m_id(id) {
}

}// namespace duk::resource