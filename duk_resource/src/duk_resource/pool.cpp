//
// Created by rov on 11/29/2023.
//

#include <duk_resource/pool.h>

namespace duk::resource {

Pool::~Pool() = default;

size_t Pools::s_poolIndexCounter;

void Pools::clear() {
    for (auto it = m_pools.rbegin(); it != m_pools.rend(); it++) {
        it->reset();
    }
}

Pools::~Pools() {
    clear();
}

}// namespace duk::resource
