//
// Created by rov on 11/20/2023.
//

#include <duk_pool/resource.h>
#include <random>

namespace duk::pool {

ResourceId ResourceId::generate() {
    static std::random_device              rd;
    static std::mt19937_64                  gen(rd());
    static std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());
    return ResourceId{dis(gen)};
}

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

uint64_t ResourceId::value() const {
    return m_id;
}
}