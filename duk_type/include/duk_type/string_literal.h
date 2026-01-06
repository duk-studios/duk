//
// Created by rov on 08/12/2025.
//

#ifndef DUK_TYPE_STRING_LITERAL_H
#define DUK_TYPE_STRING_LITERAL_H

#include <string>

namespace duk::type {

template<auto N>
struct string_literal {
    consteval string_literal(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];
};

}// namespace duk::type

#endif//DUK_TYPE_STRING_LITERAL_H