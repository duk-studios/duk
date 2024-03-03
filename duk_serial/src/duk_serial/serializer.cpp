/// 24/02/2024
/// serializer.cpp

#include <duk_serial/serializer.h>

namespace duk::serial {

MemberDescription::MemberDescription(const char* name)
    : m_name(name)
    , m_index(0) {
}

MemberDescription::MemberDescription(size_t index)
    : m_name(nullptr)
    , m_index(index) {
}

const char* MemberDescription::name() const {
    return m_name;
}

size_t MemberDescription::index() const {
    return m_index;
}

}// namespace duk::serial