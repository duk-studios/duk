/// 11/11/2023
/// generator_utils.h

#ifndef DUK_MATERIAL_GENERATOR_GENERATOR_UTILS_H
#define DUK_MATERIAL_GENERATOR_GENERATOR_UTILS_H

#include <duk_macros/macros.h>

#include <sstream>
#include <string>

namespace duk::material_generator::utils {

template<typename T, typename P>
DUK_NO_DISCARD std::string generate_for_each(const T& container, P predicate, const char* separator = ",", bool onePerLine = true) {
    std::ostringstream oss;

    bool needsSeparator = false;
    for (const auto& element: container) {
        if (needsSeparator) {
            oss << separator;
            if (onePerLine) {
                oss << std::endl;
            }
        }
        needsSeparator = true;
        oss << predicate(element);
    }

    return oss.str();
}

}// namespace duk::material_generator::utils

#endif// DUK_MATERIAL_GENERATOR_GENERATOR_UTILS_H
