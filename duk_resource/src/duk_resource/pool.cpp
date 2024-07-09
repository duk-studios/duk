//
// Created by rov on 11/29/2023.
//

#include <duk_resource/pool.h>

namespace duk::resource {

Pool::~Pool() = default;

std::unordered_map<std::string, size_t> g_poolTypeNameToIndex = {};

bool Pools::contains(Id id) const {
    for (const auto& pool: m_pools) {
        if (pool->contains(id)) {
            return true;
        }
    }
    return false;
}

void Pools::clear() {
    for (auto it = m_pools.rbegin(); it != m_pools.rend(); ++it) {
        auto& pool = *it;
        pool->clean();
    }
}

std::unordered_map<std::string, size_t>& Pools::pool_type_name_to_index() {
    return g_poolTypeNameToIndex;
}

Pools::~Pools() {
    clear();
}

}// namespace duk::resource
