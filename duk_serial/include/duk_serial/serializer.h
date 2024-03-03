/// 24/02/2024
/// serializer.h

#ifndef DUK_SERIAL_SERIALIZER_H
#define DUK_SERIAL_SERIALIZER_H

#include <string>
#include <vector>

namespace duk::serial {

class MemberDescription {
public:
    MemberDescription(const char* name);

    MemberDescription(size_t index);

    const char* name() const;

    size_t index() const;

private:
    const char* m_name;
    size_t m_index;
};

}// namespace duk::serial

#endif// DUK_SERIAL_SERIALIZER_H
