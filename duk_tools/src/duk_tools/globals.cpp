//
// Created by Ricardo on 05/07/2024.
//
#include <duk_tools/globals.h>

namespace duk::tools {

namespace detail {

std::unordered_map<uint64_t, uint32_t> g_hashToIndex;

}

Globals::~Globals() {
    clear();
}

void Globals::clear() {
    for (auto it = m_globals.rbegin(); it != m_globals.rend(); ++it) {
        it->reset();
    }
}

std::unordered_map<uint64_t, uint32_t>& Globals::hash_to_index() {
    return detail::g_hashToIndex;
}

}// namespace duk::tools
