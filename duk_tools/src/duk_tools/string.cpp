//
// Created by Ricardo on 24/01/2024.
//
#include <duk_tools/string.h>

namespace duk::tools {

std::string snake_to_pascal(const std::string& snakeCase) {
    std::string pascalCase;
    bool capitalize = true;

    for (char c : snakeCase) {
        if (c == '_') {
            capitalize = true;
            continue;
        }
        if (capitalize) {
            pascalCase += std::toupper(c);
            capitalize = false;
            continue;
        }
        pascalCase += c;
    }

    return pascalCase;
}

}