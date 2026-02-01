//
// Created by rov on 11/20/2023.
//

#include <random>
#include <duk_resource/handle.h>

namespace duk::resource {

uint64_t Id::value() const {
    return m_id;
}

bool Id::valid() const {
    return m_id != 0;
}

Id generate_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());// mersenne_twister_engine seeded with rd()
    static std::uniform_int_distribution distrib(kMaxBuiltInResourceId.value() + 1, std::numeric_limits<uint64_t>::max());
    return Id(distrib(gen));
}

}// namespace duk::resource