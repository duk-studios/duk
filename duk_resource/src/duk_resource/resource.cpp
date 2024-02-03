//
// Created by rov on 11/20/2023.
//

#include <duk_resource/resource.h>

namespace duk::resource {

Id::Id() :
        Id(0) {

}

Id::Id(uint64_t id) :
        m_id(id) {

}

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

}