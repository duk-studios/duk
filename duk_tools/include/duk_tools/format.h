/// 23/04/2023
/// format.h

#ifndef DUK_LOG_FORMAT_H
#define DUK_LOG_FORMAT_H

#include <string>
#include <sstream>
#include <vector>

namespace duk::tools {

template<typename T>
inline std::string to_str(const T& value) {
    return std::to_string(value);
}

inline std::string to_str(bool value) {
    return value ? "true" : "false";
}

inline std::string to_str(const std::string& value) {
    return value;
}

inline std::string to_str(const char* value) {
    return value;
}

template<typename ...Args>
inline  std::string format(const std::string& base, Args&& ... args) {
    if constexpr (sizeof...(args) == 0) {
        return base;
    }

    std::vector<std::string> tokens;
    tokens.reserve(sizeof...(args));
    (tokens.push_back(to_str(std::forward<Args>(args))), ...);

    std::ostringstream oss;

    size_t tokenStart = 0;
    size_t tokenEnd = 0;

    while ((tokenStart = base.find('{', tokenStart))) {

        //TODO: allow n-braces

        auto substr = base.substr(tokenEnd, tokenStart - tokenEnd);

        oss << substr;

        auto current = oss.str();

        if (tokenStart == std::string::npos) {
            break;
        }

        tokenEnd = base.find('}', tokenStart);

        if (tokenEnd == std::string::npos) {
            break;
        }

        auto contentStart = tokenStart + 1;

        auto tokenContent = base.substr(contentStart, tokenEnd - contentStart);

        auto tokenIndex = std::stoi(tokenContent);

        oss << tokens.at(tokenIndex);

        tokenStart = ++tokenEnd;
    }
    return oss.str();
}

}

#endif // DUK_LOG_FORMAT_H

