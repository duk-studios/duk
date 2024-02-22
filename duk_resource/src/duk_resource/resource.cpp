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

Id Resource::id() const {
    return m_id;
}

size_t Resource::use_count() const {
    return m_resource.use_count();
}

bool Resource::valid() const {
    return m_resource && m_id.value() != 0;
}

Resource::operator bool() const {
    return valid();
}

void Resource::reset() {
    m_resource.reset();
    m_id = Id(0);
}

}