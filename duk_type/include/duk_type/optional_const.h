//
// Created by rov on 06/12/2025.
//

#ifndef DUK_OPTIONAL_CONST_H
#define DUK_OPTIONAL_CONST_H

namespace duk::type {

template<typename T, bool isConst>
struct optional_const {};

template<typename T>
struct optional_const<T, true> {
    using type = const T;
};

template<typename T>
struct optional_const<T, false> {
    using type = T;
};

template<typename T, bool isConst>
using optional_const_t = typename optional_const<T, isConst>::type;

}

#endif //DUK_OPTIONAL_CONST_H