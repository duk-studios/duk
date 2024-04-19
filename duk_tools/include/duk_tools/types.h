/// 21/02/2024
/// types.h

#ifndef DUK_TOOLS_TYPES_H
#define DUK_TOOLS_TYPES_H

#include <string>
#include <typeindex>

namespace duk::tools {

std::string demangle(const char* value);

std::string type_name_of(const std::type_info& info);

template<typename T>
const std::string& type_name_of() {
    static const std::string typeName = type_name_of(typeid(T));
    return typeName;
}

template<typename T, bool isConst>
struct maybe_const {};

template<typename T>
struct maybe_const<T, true> {
    using type = const T;
};

template<typename T>
struct maybe_const<T, false> {
    using type = T;
};

template<typename T, bool isConst>
using maybe_const_t = typename maybe_const<T, isConst>::type;

}// namespace duk::tools

#endif// DUK_TOOLS_TYPES_H
