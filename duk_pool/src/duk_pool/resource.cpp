//
// Created by rov on 11/20/2023.
//

#include <duk_pool/resource.h>

namespace duk::pool {

ResourceId::ResourceId() :
        ResourceId(0) {

}

ResourceId::ResourceId(uint64_t id) :
        m_id(id) {

}

bool ResourceId::operator==(const ResourceId& rhs) const {
    return m_id == rhs.m_id;
}

bool ResourceId::operator!=(const ResourceId& rhs) const {
    return m_id != rhs.m_id;
}

bool ResourceId::operator<(const ResourceId& rhs) const {
    return m_id < rhs.m_id;
}

uint64_t ResourceId::value() const {
    return m_id;
}

}